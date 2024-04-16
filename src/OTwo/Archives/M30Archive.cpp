#include <OTwo/Archives/M30Archive.hpp>
#include <Genode/Utilities/StringHelper.hpp>
#include <SFML/System/MemoryInputStream.hpp>

bool M30Archive::LoadFromFile(const std::string& fileName)
{
    if (!Archive::LoadFromFile(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
        return false;

    if (!m_fileStream.open(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
        return false;

    if (m_fileStream.seek(0) == -1)
        return false;

    if (!ReadStream(&m_header, sizeof(m_header)))
        return false;

    if (const auto signature = std::string(m_header.Signature, 3); signature != "M30")
        return false;

    const auto entries = GetFileEntries();
    return !entries.empty();
}

Gx::ResourcePtr<sf::InputStream> M30Archive::Open(const std::string &fileName) const
{
    const auto header = GetFileInfo(fileName);
    if (!header)
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");

    const auto data = new Gx::Uint8[header->GetSize()];
    if (const auto read = ReadFile(dynamic_cast<FileInfo&>(*header), data, header->GetSize()); read <= 0)
        delete[] data;

    const auto stream = new sf::MemoryInputStream();
    stream->open(data, header->GetSize());

    return {
        stream,
        [data] (const sf::InputStream *ms) {
            delete ms;
            delete[] data;
        }
    };
}

Gx::ResourcePtr<sf::InputStream> M30Archive::Open(unsigned int index) const
{
    const auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bounds for this archive");

    const auto header = &iterator->second;
    const auto data = new Gx::Uint8[header->GetSize()];

    if (const auto read = ReadFile(*header, data, header->GetSize()); read <= 0)
        delete[] data;

    const auto stream = new sf::MemoryInputStream();
    stream->open(data, header->GetSize());

    return {
        stream,
        [data] (const sf::InputStream *ms) {
            delete ms;
            delete[] data;
        }
    };
}

std::unique_ptr<Gx::FileInfo> M30Archive::GetFileInfo(const std::string &fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return std::make_unique<FileInfo>(header);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");
}

Gx::Int64 M30Archive::ReadFile(const unsigned int index, void *data, const Gx::Int64 size) const
{
    const auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bounds for this archive");

    return ReadFile(iterator->second, data, size);
}

Gx::Int64 M30Archive::ReadFile(const std::string &fileName, void *data, const Gx::Int64 size) const
{
    const auto header = GetFileInfo(fileName);
    if (!header)
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");

    return ReadFile(dynamic_cast<FileInfo&>(*header), data, size);
}

bool M30Archive::Contains(const std::string& name) const
{
    return std::any_of(m_entries.begin(), m_entries.end(), [name] (auto pair) { return pair.second.GetName() == name; });
}

Gx::Int64 M30Archive::GetFileSize(const std::string &fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return header.GetSize();
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");
}

std::vector<Gx::FileInfo> M30Archive::GetFileEntries() const
{
    std::vector<Gx::FileInfo> result;

    m_entries.clear();
    if (m_fileStream.seek(m_header.SampleOffset) != -1)
        return result;

    for (unsigned int i = 0; i < m_header.SampleCount; i++)
    {
        const auto offset = m_fileStream.tell();
        if (offset == -1)
            continue;

        auto sampleHeader = M30SampleHeader();
        if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
            continue;

        if (m_fileStream.seek(m_fileStream.tell() + sampleHeader.Size) == -1)
            continue;

        auto entry = FileInfo(
            *this,
            Gx::StringHelper::Trim(std::string(sampleHeader.Name, sizeof(sampleHeader.Name))),
            sampleHeader.Size,
            i,
            offset
        );

        m_entries[i] = entry;
        result.push_back(entry);
    }

    return result;
}

bool M30Archive::ReadStream(void* data, const Gx::Uint64 size) const
{
    const auto read = m_fileStream.read(data, static_cast<std::int64_t>(size));
    return read == size;
}

Gx::Int64 M30Archive::ReadFile(const FileInfo &entry, void *data, Gx::Int64 size) const
{
    auto sampleHeader = M30SampleHeader();
    if (m_fileStream.seek(static_cast<Gx::Int64>(entry.GetOffset())) == -1)
        return -1;

    if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
        return -1;

    const auto encodedData = new Gx::Uint8[sampleHeader.Size];
    const auto read = m_fileStream.read(encodedData, sampleHeader.Size);

    if (read <= 0)
    {
        delete[] encodedData;
        return -1;
    }

    if (size > read)
        size = read;

    const auto decodedData = DecodeSample(encodedData, sampleHeader.Size, m_header.EncodingCode);
    memcpy(data, decodedData, size);

    delete[] decodedData;
    delete[] encodedData;

    return read;
}

Gx::Uint8* M30Archive::DecodeSample(const Gx::Uint8* encoded, int length, int encodingCode)
{
    auto* sample = new Gx::Uint8[length];
    if (encodingCode == 16) // nami
    {
        for (unsigned int i = 0; i + 3 < length; i += 4)
        {
            sample[i + 0] = 'n' ^ encoded[i + 0];
            sample[i + 1] = 'a' ^ encoded[i + 1];
            sample[i + 2] = 'm' ^ encoded[i + 2];
            sample[i + 3] = 'i' ^ encoded[i + 3];
        }
    }
    else
    {
        memcpy(sample, encoded, length);
    }

    return sample;
}
