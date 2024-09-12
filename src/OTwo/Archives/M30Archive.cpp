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

Gx::ResourcePtr<sf::InputStream> M30Archive::Open(const std::string& fileName) const
{
    const auto header = GetFileInfo(fileName);
    if (!header)
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");

    const auto data = new Gx::Uint8[header->GetSize()];
    if (const auto read = ReadFile(dynamic_cast<FileInfo&>(*header), data, header->GetSize()); read <= 0)
        delete[] data;

    const auto stream = new sf::MemoryInputStream(data, header->GetSize());
    return {
        stream,
        [data] (const sf::InputStream* ms) {
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

    const auto stream = new sf::MemoryInputStream(data, header->GetSize());
    return {
        stream,
        [data] (const sf::InputStream* ms) {
            delete ms;
            delete[] data;
        }
    };
}

std::unique_ptr<Gx::FileInfo> M30Archive::GetFileInfo(const std::string& fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return std::make_unique<FileInfo>(header);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

Gx::Int64 M30Archive::ReadFile(const unsigned int index, void* data, const Gx::Int64 size) const
{
    const auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bounds for this archive");

    return ReadFile(iterator->second, data, size);
}

Gx::Int64 M30Archive::ReadFile(const std::string& fileName, void* data, const Gx::Int64 size) const
{
    const auto header = GetFileInfo(fileName);
    if (!header)
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");

    return ReadFile(dynamic_cast<FileInfo&>(*header), data, size);
}

bool M30Archive::Contains(const std::string& name) const
{
    return std::any_of(m_entries.begin(), m_entries.end(), [name] (auto pair) { return pair.second.GetName() == name; });
}

Gx::Int64 M30Archive::GetFileSize(const std::string& fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return header.GetSize();
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

std::vector<std::unique_ptr<Gx::FileInfo>> M30Archive::GetFileEntries() const
{
    std::vector<std::unique_ptr<Gx::FileInfo>> result;

    m_entries.clear();
    if (!m_fileStream.seek(m_header.SampleOffset).has_value())
        return result;

    for (unsigned int i = 0; i < m_header.SampleCount; i++)
    {
        const auto offset = m_fileStream.tell();
        if (!offset.has_value())
            continue;

        auto sampleHeader = M30SampleHeader();
        if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
            continue;

        if (!m_fileStream.seek(m_fileStream.tell().value() + sampleHeader.Size).has_value())
            continue;

        unsigned int reference = sampleHeader.Reference + (sampleHeader.CodecCode == 0 ? 1000 : 0);
        auto entry = FileInfo(
            *this,
            Gx::StringHelper::Trim(std::string(sampleHeader.Name, sizeof(sampleHeader.Name))),
            sampleHeader.Size,
            reference,
            offset.value()
        );

        m_entries[reference] = entry;
        result.push_back(std::make_unique<FileInfo>(entry));
    }

    return result;
}

bool M30Archive::ReadStream(void* data, const Gx::Uint64 size) const
{
    const auto read = m_fileStream.read(data, static_cast<std::int64_t>(size));
    return read == size;
}

Gx::Int64 M30Archive::ReadFile(const FileInfo& entry, void* data, Gx::Int64 size) const
{
    auto sampleHeader = M30SampleHeader();
    if (m_fileStream.seek(static_cast<Gx::Int64>(entry.GetOffset())) == -1)
        return -1;

    if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
        return -1;

    auto sampleData = std::vector<Gx::Uint8>(sampleHeader.Size);
    const auto read = m_fileStream.read(&sampleData[0], sampleHeader.Size);

    if (!read.has_value())
        return -1;

    if (size > read)
        size = read.value();

    DecodeSample(sampleData, m_header.EncodingCode);
    memcpy(data, &sampleData[0], size);

    return read.value();
}

void M30Archive::DecodeSample(std::vector<Gx::Uint8> &data, const int encodingCode)
{
    if (encodingCode == 16) // nami
    {
        for (unsigned int i = 0; i + 3 < data.size(); i += 4)
        {
            data[i + 0] = 'n' ^ data[i + 0];
            data[i + 1] = 'a' ^ data[i + 1];
            data[i + 2] = 'm' ^ data[i + 2];
            data[i + 3] = 'i' ^ data[i + 3];
        }
    }
}
