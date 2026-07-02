#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>
#include <CXO2/Archives/OjmArchive.hpp>
#include <CXO2/Contexts/GameContext.hpp>

#include <SFML/System/MemoryInputStream.hpp>
#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    O2JamChartLoader::O2JamChartLoader(const GameContext& context) :
        m_mode(context.GetMode()),
        m_difficulty(context.GetDifficulty())
    {
    }

    Gx::ResourcePtr<Chart> O2JamChartLoader::LoadFromMetadata(const O2JamChartMetadata& meta, const Gx::ResourceContext& ctx) const
    {
        auto chart = LoadFromFile(meta.Source, ctx);
        chart->Source = meta.Source;

        return chart;
    }

    Gx::ResourcePtr<Chart> O2JamChartLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException("Failed to open the file: " + fileName.string());

        auto& inputStream = *stream.get();
        auto chart        = LoadFromStream(inputStream, Gx::ResourceContext::Rebind(ctx, fileName.string()));
        chart->Source     = fileName.string();

        return chart;
    }

    Gx::ResourcePtr<Chart> O2JamChartLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto stream = sf::MemoryInputStream(data, size);
        return LoadFromStream(stream, ctx);
    }

    Gx::ResourcePtr<Chart> O2JamChartLoader::LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const
    {
        const auto decryptedStream = Decrypt(Gx::ResourcePtr<sf::InputStream>(&input, [] (auto) {}));
        auto& stream = *decryptedStream.get();

        auto chart = std::make_unique<Chart>();
        chart->Source = ctx.GetID();

        auto metadata = O2JamChartMetadata{};
        if (!stream.seek(0).has_value())
            throw Gx::ResourceLoadException("Failed to open the stream");

        if (stream.read(&metadata, O2JamChartMetadata::Size) != O2JamChartMetadata::Size)
            return nullptr;

        chart->SetMetadata(metadata.ToChartMetadata());
        if (metadata.CoverSize > 0 && m_onCoverLoaded)
        {
            chart->SetCover(LoadCoverArt(stream, metadata, ctx));
            if (m_onCoverLoaded)
                m_onCoverLoaded(chart->GetCover());
        }

        if (metadata.ThumbnailSize > 0 && m_onThumbnailLoaded)
        {
            chart->SetThumbnail(LoadThumbnail(stream, metadata, ctx));
            m_onThumbnailLoaded(chart->GetThumbnail());
        }

        auto loadSamples = [&chart] (const OjmArchive* archive)
        {
            if (!archive)
                return;

            for (const auto& e : archive->GetFileEntries())
            {
                const auto entry = dynamic_cast<const FileInfo*>(e.get());
                if (!entry)
                    continue;

                auto payload = std::vector<std::uint8_t>(entry->GetSize());
                if (!entry->Read(&payload[0]).has_value())
                    continue;

                const auto loader = Gx::ResourceLoaderFactory::CreateLoader<sf::SoundBuffer>();
                if (auto buffer = loader->LoadFromMemory(&payload[0], entry->GetSize(), Gx::ResourceContext::Default); buffer)
                    chart->AddSample(entry->GetIndex(), std::move(buffer));
            }
        };

        // Use resource manager at first attmpt
        if (ctx.Available())
        {
            if (const auto archive = ctx.Find<OjmArchive>(metadata.OJM); archive)
                loadSamples(archive);
        }

        // Archive not loaded from resource manager, try to open the file directly
        if (chart->GetSampleCount() == 0)
        {
            auto archive = OjmArchive();
            if (archive.LoadFromFile(Gx::StringHelper::Trim(metadata.OJM).toAnsiString()))
                loadSamples(&archive);
        }

        for (unsigned int diff = 0; diff < 3; diff++)
        {
            const auto difficulty = static_cast<Difficulty>(diff);
            if (difficulty != m_difficulty)
                continue;

            std::uint32_t offset = 0;
            std::uint32_t blockCount = 0;
            std::uint32_t size = 0;

            switch (difficulty)
            {
                case Difficulty::EX:
                    offset = metadata.BlockOffsetEx;
                    blockCount = metadata.BlockCountEx;
                    size = metadata.BlockOffsetNx - offset;
                    break;
                case Difficulty::NX:
                    offset = metadata.BlockOffsetNx;
                    blockCount = metadata.BlockCountNx;
                    size = metadata.BlockOffsetHx - offset;
                    break;
                case Difficulty::HX:
                    offset = metadata.BlockOffsetHx;
                    blockCount = metadata.BlockCountHx;
                    size = metadata.CoverOffset - offset;
                    break;
                case Difficulty::MX:
                    break;
            }

            if (offset == 0 || blockCount == 0)
                continue;

            if (!stream.seek(offset).has_value())
                continue;

            for (unsigned int p = 0; p < blockCount; p++)
            {
                auto block = NoteBlockHeader();
                if (stream.read(&block, sizeof(NoteBlockHeader)) != sizeof(NoteBlockHeader))
                    throw Gx::ResourceLoadException("Failed to read the note block");

                // ====================================================================================================
                // WARNING: Channel::Background will not map, but this will retain the original channel value.
                const auto channel = static_cast<Chart::Channel>(block.Channel);
                // ====================================================================================================

                for (unsigned int i = 0; i < block.EventCount; i++)
                {
                    double position = block.Measure + (static_cast<double>(i) / static_cast<double>(block.EventCount));
                    if (auto fractions = chart->GetMeasureFractions(difficulty); fractions.size() > 0)
                    {
                        for (auto [m, fraction] : fractions)
                        {
                            if (m > block.Measure)
                                break;

                            position -= 1.f - fraction;
                        }
                    }

                    auto ev = Chart::Event(channel, position);
                    if (channel == Chart::Channel::BPM || channel == Chart::Channel::Measure)
                    {
                        float value;
                        if (stream.read(&value, sizeof(value)) != sizeof(value))
                            throw Gx::ResourceLoadException("Failed to read time event value");

                        if (value == 0.f)
                            continue;

                        if (ev.Channel == Chart::Channel::Measure)
                            chart->SetMeasureFraction(difficulty, block.Measure + 1, value);

                        chart->AddEvent<Chart::TimeEvent>(difficulty, Chart::TimeEvent(ev, value));
                        continue;
                    }

                    auto note = NoteEventHeader();
                    if (stream.read(&note, sizeof(NoteEventHeader)) != sizeof(NoteEventHeader))
                        throw Gx::ResourceLoadException("Failed to read note event id");

                    // Ignore padding event
                    if (note.ID == 0)
                        continue;

                    // Volume value is between 1 ~ 16.
                    // It needs to be converted to 0.f ~ 100.f range
                    auto volume = static_cast<float>((note.Audio >> 4) & 0x0F);
                    volume = volume == 0 ? 100.f : ((volume / 16.f) * 100.f);

                    // Pan value is between 1 ~ 15 where 8 is the center
                    // It needs to be converted to -1.f to 1.f range where 0.f is the center
                    auto pan = static_cast<float>(note.Audio & 0x0F);
                    pan = pan == 0 ? 8 : pan;
                    pan = ((pan - 1) / 14.0f) * 2.0f - 1.0f;

                    // Resolve the sample type and ref id
                    auto sampleType = note.Type % 8 > 3 ? Chart::SampleType::Background : Chart::SampleType::KeySound;
                    const int id = (note.ID - 1) + (sampleType == Chart::SampleType::Background ? 1000 : 0);

                    // Resolve note type
                    auto type = Chart::NoteType::Tap;
                    switch(note.Type % 4)
                    {
                        case 2: type = Chart::NoteType::Hold;    break;
                        case 3: type = Chart::NoteType::Release; break;
                        default: break;
                    }

                    chart->AddEvent<Chart::NoteEvent>(difficulty, Chart::NoteEvent(ev, id, volume, pan, type, sampleType, chart->GetSample(id)));
                }
            }
        }

        chart->SortEvents();
        return chart;
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadThumbnail(const std::string& source, const Gx::ResourceContext& ctx)
    {
        const auto loader = O2JamChartMetadataLoader();
        if (const auto metadata = loader.LoadFromFile(source, ctx))
            return LoadThumbnail(*metadata, ctx);

        return nullptr;
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadCoverArt(const std::string& source, const Gx::ResourceContext& ctx)
    {
        const auto loader = O2JamChartMetadataLoader();
        if (const auto metadata = loader.LoadFromFile(source, ctx))
            return LoadCoverArt(*metadata, ctx);

        return nullptr;
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadThumbnail(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx)
    {
        const auto fs = Gx::FileSystem::Open(metadata.Source);
        return LoadThumbnail(*fs, metadata, ctx);
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadCoverArt(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx)
    {
        const auto fs = Gx::FileSystem::Open(metadata.Source);
        return LoadCoverArt(*fs, metadata, ctx);
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadThumbnail(sf::InputStream& input, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx)
    {
        const auto decryptedStream = Decrypt(Gx::ResourcePtr<sf::InputStream>(&input, [] (auto) {}));
        auto& stream = *decryptedStream.get();

        if (metadata.ThumbnailSize == 0)
            return nullptr;

        if (!stream.seek(metadata.CoverOffset + metadata.CoverSize).has_value())
            return nullptr;

        auto data = std::vector<std::uint8_t>(metadata.ThumbnailSize);
        if (const auto read = stream.read(data.data(), metadata.ThumbnailSize); !read.has_value())
            return nullptr;

        auto image  = std::make_unique<sf::Image>();
        if (image->loadFromMemory(data.data(), metadata.ThumbnailSize))
        {
            for (unsigned y = 0; y < image->getSize().y; ++y)
            {
                for (unsigned x = 0; x < image->getSize().x; ++x)
                {
                    auto p = image->getPixel({x, y});
                    p.a = 255;

                    image->setPixel({x, y}, p);
                }
            }

            return image;
        }

        return nullptr;
    }

    Gx::ResourcePtr<sf::Image> O2JamChartLoader::LoadCoverArt(sf::InputStream& input, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx)
    {
        const auto decryptedStream = Decrypt(Gx::ResourcePtr<sf::InputStream>(&input, [] (auto) {}));
        auto& stream = *decryptedStream.get();

        if (metadata.CoverSize == 0)
            return nullptr;

        if (!stream.seek(metadata.CoverOffset).has_value())
            return nullptr;

        auto data = std::vector<std::uint8_t>(metadata.CoverSize);
        if (stream.read(data.data(), metadata.CoverSize) != metadata.CoverSize)
            return nullptr;

        auto image  = std::make_unique<sf::Image>();
        if (!image->loadFromMemory(data.data(), metadata.CoverSize))
            return nullptr;

        return image;
    }

    Gx::ResourcePtr<sf::InputStream> O2JamChartLoader::Decrypt(Gx::ResourcePtr<sf::InputStream> stream)
    {
        // Early return if stream is null
        if (!stream)
            return stream;

        auto& input = *stream.get();

        // Read entire stream into memory
        const auto totalSizeOpt = input.getSize();
        if (!totalSizeOpt.has_value())
        {
            auto _ = stream->seek(0);
            return stream;
        }

        const std::size_t totalSize = totalSizeOpt.value();
        if (totalSize < 8)
        {
            auto _ = stream->seek(0);
            return stream;
        }

        if (!input.seek(0).has_value())
        {
            auto _ = stream->seek(0);
            return stream;
        }

        auto data = std::vector<std::uint8_t>(totalSize);
        if (input.read(data.data(), static_cast<std::int64_t>(totalSize)) != totalSize)
        {
            auto _ = stream->seek(0);
            return stream;
        }

        // Validate signature "new"
        const std::string signature(reinterpret_cast<const char*>(data.data()), 3);
        if (signature != "new")
        {
            auto _ = stream->seek(0);
            return stream;
        }

        // Read block info and keys
        const std::uint8_t blockSize  = data[3];
        const std::uint8_t mainKey    = data[4];
        const std::uint8_t midKey     = data[5];
        const std::uint8_t initialKey = data[6];

        if (blockSize == 0)
        {
            auto _ = stream->seek(0);
            return stream;
        }

        // Prepare encrypt key buffer
        std::vector<std::uint8_t> encryptKey(blockSize, mainKey);
        encryptKey[0] = initialKey;
        encryptKey[static_cast<std::size_t>(std::floor(static_cast<float>(blockSize) / 2.f))] = midKey;

        // Discard 8-byte header and decrypt the rest reading backwards
        const std::size_t payloadSize = totalSize - 8;
        auto output = new std::uint8_t[payloadSize];

        for (std::size_t i = 0; i < payloadSize; i += blockSize)
        {
            for (std::size_t j = 0; j < blockSize; j++)
            {
                const std::size_t offset = i + j;
                if (offset >= payloadSize)
                    break;

                const std::size_t srcIndex = (totalSize - 1) - offset; // read backward from end of file
                output[offset] = static_cast<std::uint8_t>(data[srcIndex] ^ encryptKey[j]);
            }
        }

        return Gx::ResourcePtr<sf::InputStream>(new sf::MemoryInputStream(output, payloadSize), [output] (const sf::InputStream* ms)
        {
            delete[] output;
            delete ms;
        });
    }

    void O2JamChartLoader::SetCoverLoadCallback(const std::function<void(const sf::Image*)> &onCoverLoaded)
    {
        m_onCoverLoaded = onCoverLoaded;
    }

    void O2JamChartLoader::SetThumbnailLoadCallback(const std::function<void(const sf::Image*)> &onThumbnailLoaded)
    {
        m_onThumbnailLoaded = onThumbnailLoaded;
    }
}
