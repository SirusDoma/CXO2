#include <OTwo/Archives/OmcArchive.hpp>

#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <sstream>

bool OmcArchive::LoadFromFile(const std::string& fileName)
{
    if (!Archive::LoadFromFile(fileName))
        return false;

    if (!m_fileStream.open(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
        return false;

    if (!m_fileStream.seek(0).has_value())
        return false;

    if (!ReadStream(&m_header, sizeof(m_header)))
        return false;

    if (const auto signature = std::string(m_header.Signature, 3); signature != "OMC" && signature != "OJM") {
        return false;
    }

    const auto entries = GetFileEntries();
    return !entries.empty(); // == m_header.FxCount + m_header.BgCount;
}

Gx::ResourcePtr<sf::InputStream> OmcArchive::Open(const unsigned int index) const
{
    const auto it = m_entries.find(index);
    if (it == m_entries.end())
        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bound for this archive");

    const auto header = it->second;
    const auto data = new std::uint8_t[header.GetSize()];
    if (!ReadFile(index, data, header.GetSize()).has_value())
        delete[] data;

    return Gx::ResourcePtr<sf::InputStream>(new sf::MemoryInputStream(data, header.GetSize()), [data] (const sf::InputStream* ms)
    {
        delete[] data;
        delete ms;
    });
}

Gx::ResourcePtr<sf::InputStream> OmcArchive::Open(const std::string& fileName) const
{
    for (auto const& [index, header] : m_entries)
    {
        if (header.GetName() != fileName)
            continue;

        const auto data = new std::uint8_t[header.GetSize()];
        if (!ReadFile(index, data, header.GetSize()).has_value())
        {
            delete[] data;
            throw Gx::ResourceLoadException(fileName, "Failed to load the specified archive entry file");
        }

        return Gx::ResourcePtr<sf::InputStream>(new sf::MemoryInputStream(data, header.GetSize()), [data] (const sf::InputStream* ms)
        {
            delete[] data;
            delete ms;
        });
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

bool OmcArchive::Contains(const std::string& name) const
{
    return std::any_of(m_entries.begin(), m_entries.end(), [name] (auto pair) { return pair.second.GetName() == name; });
}

std::vector<std::unique_ptr<Gx::FileInfo>> OmcArchive::GetFileEntries() const
{
    std::vector<std::unique_ptr<Gx::FileInfo>> result;
    m_entries.clear();

    if (!m_fileStream.seek(m_header.FxStartOffset).has_value())
        return result;

    for (unsigned int i = 0; i < m_header.FxCount; i++)
    {
        const auto offset  = m_fileStream.tell();
        if (!offset.has_value())
            continue;

        auto waveHeader = OmcWaveHeader();
        if (!ReadStream(&waveHeader, sizeof(waveHeader)))
            continue;

        if (!m_fileStream.seek(m_fileStream.tell().value() + waveHeader.ChunkSize).has_value())
            continue;

        auto entry = FileInfo(*this, Gx::StringHelper::Trim(waveHeader.Name), waveHeader.ChunkSize + 44, i, offset.value());
        m_entries[i] = entry;

        result.push_back(std::make_unique<FileInfo>(entry));
    }

    if (!m_fileStream.seek(m_header.BgStartOffset).has_value())
        return result;

    for (unsigned int i = 0; i < m_header.BgCount; i++)
    {
        const auto offset  = m_fileStream.tell();
        if (!offset.has_value())
            continue;

        auto oggHeader = OmcOggHeader();
        if (!ReadStream(&oggHeader, sizeof(oggHeader)))
            continue;

        if (!m_fileStream.seek(m_fileStream.tell().value() + oggHeader.Size).has_value())
            continue;

        auto entry = FileInfo(*this, Gx::StringHelper::Trim(oggHeader.Name), oggHeader.Size, i + 1000, offset.value());
        m_entries[i + 1000] = entry;

        result.push_back(std::make_unique<FileInfo>(entry));
    }

    return result;
}


std::unique_ptr<Gx::FileInfo> OmcArchive::GetFileInfo(const std::string& fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return std::make_unique<FileInfo>(header);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

std::optional<std::size_t> OmcArchive::ReadFile(const unsigned int index, void* data, std::size_t size) const
{
    const auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bound for this archive");

    if (index < 1000)
    {
        if (!m_fileStream.seek(m_header.FxStartOffset).has_value())
            return std::nullopt;

        // We can't seek straight to desired sample because we need to calculate these counters
        int accKeyByte = 0xFF;
        int accCounter = 0;

        for (unsigned int i = 0; i < m_header.FxCount; i++)
        {
            auto waveHeader = OmcWaveHeader();
            if (!ReadStream(&waveHeader, sizeof(waveHeader)))
                throw Gx::ResourceLoadException(std::to_string(index), "Failed to read the WAV header");

            const auto encodedData = new std::uint8_t[waveHeader.ChunkSize];
            if (!ReadStream(encodedData, waveHeader.ChunkSize))
            {
                delete[] encodedData;
                throw Gx::ResourceLoadException(std::to_string(index), "Failed to read the encoded WAV data");
            }

            // Still need to decode even not desired sample to increment accKeyByte and accCounter
            const auto decodedData = DecodeWave(encodedData, waveHeader.ChunkSize, &accKeyByte, &accCounter);
            int pcm = 16, fileSize = waveHeader.ChunkSize + 36;
            if (i != index)
            {
                delete[] encodedData;
                delete[] decodedData;
                continue;
            }

            // Build complete wave sample
            std::stringstream waveStream;
            waveStream.write("RIFF", 4);
            waveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
            waveStream.write("WAVE", 4);
            waveStream.write("fmt ", 4);
            waveStream.write(reinterpret_cast<char*>(&pcm), sizeof(pcm));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.AudioFormat), sizeof(waveHeader.AudioFormat));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.ChannelCount), sizeof(waveHeader.ChannelCount));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.SampleRate), sizeof(waveHeader.SampleRate));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.BitRate), sizeof(waveHeader.BitRate));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.BlockAlign), sizeof(waveHeader.BlockAlign));
            waveStream.write(reinterpret_cast<char*>(&waveHeader.BitsPerSample), sizeof(waveHeader.BitsPerSample));
            waveStream.write("data", 4);
            waveStream.write(reinterpret_cast<char*>(&waveHeader.ChunkSize), sizeof(waveHeader.ChunkSize));
            waveStream.write(reinterpret_cast<char*>(decodedData), waveHeader.ChunkSize);

            const auto buffer = waveStream.str();
            const unsigned int read = buffer.length();

            if (size <= 0 || size > read)
                size = read;

            memcpy(data, buffer.data(), size);
            delete[] encodedData;
            delete[] decodedData;

            return read;
        }

        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bound for this archive");
    }
    else
    {
        if (!m_fileStream.seek(static_cast<std::int64_t>(iterator->second.GetOffset())).has_value())
            return std::nullopt;

        auto oggHeader = OmcOggHeader();
        if (!ReadStream(&oggHeader, sizeof(oggHeader)))
            return std::nullopt;

        if (size > oggHeader.Size)
            size = oggHeader.Size;

        return m_fileStream.read(data, size);
    }
}

std::optional<std::size_t> OmcArchive::ReadFile(const std::string& fileName, void* data, const std::size_t size) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return ReadFile(key, data, size);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

std::optional<std::size_t> OmcArchive::GetFileSize(const std::string& fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return header.GetSize();
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

std::string OmcArchive::GetExtension(const std::string& name) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == name)
            return key < 1000 ? ".wav" : ".ogg";
    }

    return "";
}

bool OmcArchive::ReadStream(void* data, const std::uint64_t size) const
{
    const auto read = m_fileStream.read(data, static_cast<std::int64_t>(size));
    return read == size;
}

static const std::uint8_t WAVE_REARRANGE_TABLE[] = {
    0x10, 0x0E, 0x02, 0x09, 0x04, 0x00, 0x07, 0x01,
    0x06, 0x08, 0x0F, 0x0A, 0x05, 0x0C, 0x03, 0x0D,
    0x0B, 0x07, 0x02, 0x0A, 0x0B, 0x03, 0x05, 0x0D,
    0x08, 0x04, 0x00, 0x0C, 0x06, 0x0F, 0x0E, 0x10,
    0x01, 0x09, 0x0C, 0x0D, 0x03, 0x00, 0x06, 0x09,
    0x0A, 0x01, 0x07, 0x08, 0x10, 0x02, 0x0B, 0x0E,
    0x04, 0x0F, 0x05, 0x08, 0x03, 0x04, 0x0D, 0x06,
    0x05, 0x0B, 0x10, 0x02, 0x0C, 0x07, 0x09, 0x0A,
    0x0F, 0x0E, 0x00, 0x01, 0x0F, 0x02, 0x0C, 0x0D,
    0x00, 0x04, 0x01, 0x05, 0x07, 0x03, 0x09, 0x10,
    0x06, 0x0B, 0x0A, 0x08, 0x0E, 0x00, 0x04, 0x0B,
    0x10, 0x0F, 0x0D, 0x0C, 0x06, 0x05, 0x07, 0x01,
    0x02, 0x03, 0x08, 0x09, 0x0A, 0x0E, 0x03, 0x10,
    0x08, 0x07, 0x06, 0x09, 0x0E, 0x0D, 0x00, 0x0A,
    0x0B, 0x04, 0x05, 0x0C, 0x02, 0x01, 0x0F, 0x04,
    0x0E, 0x10, 0x0F, 0x05, 0x08, 0x07, 0x0B, 0x00,
    0x01, 0x06, 0x02, 0x0C, 0x09, 0x03, 0x0A, 0x0D,
    0x06, 0x0D, 0x0E, 0x07, 0x10, 0x0A, 0x0B, 0x00,
    0x01, 0x0C, 0x0F, 0x02, 0x03, 0x08, 0x09, 0x04,
    0x05, 0x0A, 0x0C, 0x00, 0x08, 0x09, 0x0D, 0x03,
    0x04, 0x05, 0x10, 0x0E, 0x0F, 0x01, 0x02, 0x0B,
    0x06, 0x07, 0x05, 0x06, 0x0C, 0x04, 0x0D, 0x0F,
    0x07, 0x0E, 0x08, 0x01, 0x09, 0x02, 0x10, 0x0A,
    0x0B, 0x00, 0x03, 0x0B, 0x0F, 0x04, 0x0E, 0x03,
    0x01, 0x00, 0x02, 0x0D, 0x0C, 0x06, 0x07, 0x05,
    0x10, 0x09, 0x08, 0x0A, 0x03, 0x02, 0x01, 0x00,
    0x04, 0x0C, 0x0D, 0x0B, 0x10, 0x05, 0x06, 0x0F,
    0x0E, 0x07, 0x09, 0x0A, 0x08, 0x09, 0x0A, 0x00,
    0x07, 0x08, 0x06, 0x10, 0x03, 0x04, 0x01, 0x02,
    0x05, 0x0B, 0x0E, 0x0F, 0x0D, 0x0C, 0x0A, 0x06,
    0x09, 0x0C, 0x0B, 0x10, 0x07, 0x08, 0x00, 0x0F,
    0x03, 0x01, 0x02, 0x05, 0x0D, 0x0E, 0x04, 0x0D,
    0x00, 0x01, 0x0E, 0x02, 0x03, 0x08, 0x0B, 0x07,
    0x0C, 0x09, 0x05, 0x0A, 0x0F, 0x04, 0x06, 0x10,
    0x01, 0x0E, 0x02, 0x03, 0x0D, 0x0B, 0x07, 0x00,
    0x08, 0x0C, 0x09, 0x06, 0x0F, 0x10, 0x05, 0x0A,
    0x04, 0x00
};

std::uint8_t* OmcArchive::DecodeWave(std::uint8_t* in, const int length, int *accKeyByte, int *accCounter)
{
    auto* out = new std::uint8_t[length];
    int key = ((length % 17) << 4) + (length % 17);
    const int blockSize = length / 17;

    memcpy(out, in, length);
    for (int block = 0; block < 17; block++)
    {
        const int inOffset = blockSize * block;
        const int outOffset = blockSize * WAVE_REARRANGE_TABLE[key];

        memcpy(&out[outOffset], &in[inOffset], blockSize);
        key++;
    }

    for (int i = 0; i < length; i++)
    {
        std::uint8_t currentByte = out[i], temp = out[i];
        if (const int accXor = (*accKeyByte << *accCounter) & 0x80; accXor != 0)
            currentByte = static_cast<std::uint8_t>(~currentByte);

        out[i] = currentByte;
        *accCounter += 1;

        if (*accCounter > 7)
        {
            *accCounter = 0;
            *accKeyByte = temp;
        }
    }

    return out;
}
