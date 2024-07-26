#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/Archives/OjmArchive.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Chart> ChartLoader::LoadFromMetadata(const ChartMetadata &meta, const Gx::ResourceContext &ctx) const
{
    auto chart = LoadFromFile(meta.Source, ctx);
    chart->Source = meta.Source;

    return chart;
}

Gx::ResourcePtr<Chart> ChartLoader::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &context) const
{
    const auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream.get();
    auto chart        = LoadFromStream(inputStream, context);
    chart->Source     = fileName;

    return chart;
}

Gx::ResourcePtr<Chart> ChartLoader::LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const
{
    auto stream = sf::MemoryInputStream();
    stream.open(data, size);

    return LoadFromStream(stream, ctx);
}

Gx::ResourcePtr<Chart> ChartLoader::LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const
{
    auto chart = std::make_unique<Chart>();
    chart->Source = ctx.GetID();

    auto metadata = chart->GetMetadata();
    if (stream.seek(0) == -1)
        throw Gx::ResourceLoadException("Failed to open the stream");

    if (stream.read(&metadata, ChartMetadata::Size) != ChartMetadata::Size)
        return nullptr;

    chart->SetMetadata(metadata);
    if (metadata.CoverSize > 0)
        chart->SetCover(LoadCoverArt(stream, metadata, ctx));

    if (m_onCoverLoaded)
        m_onCoverLoaded(chart->GetCover());

    if (metadata.ThumbnailSize > 0)
        chart->SetThumbnail(LoadThumbnail(stream, metadata, ctx));

    if (m_onThumbnailLoaded)
        m_onThumbnailLoaded(chart->GetThumbnail());

    bool samplesLoaded = false;
    auto loadSamples   = [&chart] (OjmArchive *archive)
    {
        if (!archive)
            return;

        for (const auto &e : archive->GetFileEntries())
        {
            const auto entry = dynamic_cast<const FileInfo*>(&e);
            if (!entry)
                continue;

            auto payload = std::vector<Gx::Uint8>(entry->GetSize());
            if (entry->Read(&payload[0]) <= 0)
                continue;

            const auto loader = Gx::ResourceLoaderFactory::GetLoader<sf::SoundBuffer>();
            if (auto buffer = loader->LoadFromMemory(&payload, entry->GetSize(), Gx::ResourceContext::Default); buffer)
                chart->AddSample(entry->GetIndex(), std::move(buffer));
        }
    };

    if (ctx.Available())
    {
        if (const auto archive = ctx.Find<OjmArchive>(metadata.OJM); archive)
        {
            loadSamples(archive);
            samplesLoaded = true;
        }
    }

    if (!samplesLoaded)
    {
        auto archive = OjmArchive();
        if (archive.LoadFromFile(Gx::StringHelper::Trim(metadata.OJM)))
        {
            loadSamples(&archive);
            samplesLoaded = true;
        }
    }

    for (int d = 0; d < 3; d++)
    {
        const auto difficulty = static_cast<Difficulty>(d);

        Gx::Uint32 offset = 0;
        Gx::Uint32 blockCount = 0;

        switch (difficulty)
        {
            case Difficulty::EX:
                offset = metadata.BlockOffsetEx;
                blockCount = metadata.BlockCountEx;
                break;
            case Difficulty::NX:
                offset = metadata.BlockOffsetNx;
                blockCount = metadata.BlockCountNx;
                break;
            case Difficulty::HX:
                offset = metadata.BlockOffsetHx;
                blockCount = metadata.BlockCountHx;
                break;
            case Difficulty::MX:
                break;
        }

        if (offset == 0 || blockCount == 0)
            continue;

        if (stream.seek(static_cast<Gx::Int64>(offset)) == -1)
            continue;

        for (int block = 0; block < blockCount; block++)
        {
            Gx::Uint32 measure;
            Gx::Uint16 lane;
            Gx::Uint16 count;

            if (stream.read(&measure, sizeof(measure)) != sizeof(measure))
                throw Gx::ResourceLoadException("Failed to read measure at note block");

            if (stream.read(&lane, sizeof(lane)) != sizeof(lane))
                throw Gx::ResourceLoadException("Failed to read channel at note block");

            if (stream.read(&count, sizeof(count)) != sizeof(count))
                throw Gx::ResourceLoadException("Failed to read block count at note block");

            auto channel = static_cast<Chart::ChannelType>(lane);
            if (lane > 8)
                channel = Chart::ChannelType::BGM;

            for (int i = 0; i < count; i++)
            {
                const float position = static_cast<float>(measure) + (static_cast<float>(i) / static_cast<float>(count));
                const auto ev = Chart::Event{ position, channel };

                if (channel == Chart::ChannelType::BPM || channel == Chart::ChannelType::Measurement)
                {
                    std::float_t value;
                    if (stream.read(&value, sizeof(value)) != sizeof(value))
                        throw Gx::ResourceLoadException("Failed to read time event value");

                    chart->AddEvent<Chart::TimeEvent>(difficulty,
                    {
                        ev,
                        value
                    });

                    continue;
                }

                Gx::Uint16 id;
                Gx::Int8 audio;
                Chart::NoteType flag;

                if (stream.read(&id, sizeof(id)) != sizeof(id))
                    throw Gx::ResourceLoadException("Failed to read note event id");

                if (id == 0)
                    continue;

                if (stream.read(&audio, sizeof(audio)) != sizeof(audio))
                    throw Gx::ResourceLoadException("Failed to read note audio data");

                if (stream.read(&flag, sizeof(flag)) != sizeof(flag))
                    throw Gx::ResourceLoadException("Failed to read note flag data");

                // Volume value is between 1 ~ 16.
                // It needs to be converted to 0.f ~ 100.f range
                auto volume = static_cast<float>((audio >> 4) & 0x0F);
                volume = volume == 0 ? 100.f : ((volume / 16.f) * 100.f);

                // Pan value is between 1 ~ 15 where 8 is the center
                // It needs to be converted to -1.f to 1.f range where 0.f is the center
                auto pan = static_cast<float>(audio & 0x0F);
                pan = pan == 0 ? 8 : pan;
                pan = ((pan - 1) / 14.0f) * 2.0f - 1.0f;

                id = (id - 1) + (channel == Chart::ChannelType::BGM ? 1000 : 0);
                chart->AddEvent<Chart::NoteEvent>(difficulty,
                {
                    ev,
                    id,
                    volume,
                    pan,
                    flag,
                    chart->GetSample(id)
                });
            }
        }
    }

    return chart;
}

Gx::ResourcePtr<sf::Image> ChartLoader::LoadThumbnail(ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    const auto fs = Gx::FileSystem::Open(metadata.Source);
    return LoadThumbnail(*fs, metadata, ctx);
}

Gx::ResourcePtr<sf::Image> ChartLoader::LoadCoverArt(ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    const auto fs = Gx::FileSystem::Open(metadata.Source);
    return LoadCoverArt(*fs, metadata, ctx);
}

Gx::ResourcePtr<sf::Image> ChartLoader::LoadThumbnail(sf::InputStream &stream, ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (stream.seek(metadata.CoverOffset + metadata.CoverSize) == -1)
        return nullptr;

    auto data = std::vector<Gx::Uint8>(metadata.ThumbnailSize);
    if (stream.read(&data[0], metadata.ThumbnailSize) != metadata.ThumbnailSize)
        return nullptr;

    auto image  = std::make_unique<sf::Image>();
    if (!image->loadFromMemory(&data[0], metadata.ThumbnailSize))
        throw Gx::ResourceLoadException("Failed to load image.");

    return image;
}

Gx::ResourcePtr<sf::Image> ChartLoader::LoadCoverArt(sf::InputStream &stream, ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (stream.seek(metadata.CoverOffset) == -1)
        return nullptr;

    auto data = std::vector<Gx::Uint8>(metadata.CoverSize);
    if (stream.read(&data[0], metadata.CoverSize) != metadata.CoverSize)
        return nullptr;

    auto image = std::make_unique<sf::Image>();
    if (!image->loadFromMemory(&data[0], metadata.CoverSize))
        throw Gx::ResourceLoadException("Failed to load image.");

    return image;
}

void ChartLoader::SetCoverLoadCallback(const std::function<void(const sf::Image*)> &onCoverLoaded)
{
    m_onCoverLoaded = onCoverLoaded;
}

void ChartLoader::SetThumbnailLoadCallback(const std::function<void(const sf::Image*)> &onThumbnailLoaded)
{
    m_onThumbnailLoaded = onThumbnailLoaded;
}
