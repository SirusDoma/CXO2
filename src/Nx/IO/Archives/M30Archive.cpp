#include <Nx/IO/Archives/M30Archive.hpp>

M30Archive::M30Archive() :
    m_header(),
    m_fileStream(),
    m_entries()
{
}

M30Archive::~M30Archive()
{
}

bool M30Archive::Open(const std::string& fileName)
{
    if (!Archive::Open(fileName))
        return false;

    m_fileStream.open(fileName);
    m_fileStream.seek(0);

    if (!Read(&m_header, sizeof(m_header)))
        return false;

    auto signature = std::string(m_header.Signature, 3);
    if (signature != "M30")
        return false;

    auto entries = GetFileEntries();
    return entries.size() > 0;
}

bool M30Archive::Contains(const std::string& name) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == name)
            return true;
    }

    return false;
}

Gx::Int64 M30Archive::GetFile(unsigned int index, Gx::Uint8** data) const
{
    auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        return -1;

    M30SampleHeader sampleHeader;
    m_fileStream.seek(iterator->second.Offset);
    if (!Read(&sampleHeader, sizeof(sampleHeader)))
        return -1;

    auto encodedData = new Gx::Uint8[sampleHeader.Size];
    auto read = m_fileStream.read(encodedData, sampleHeader.Size);

    if (read <= 0)
    {
        delete[] encodedData;
        return -1;
    }

    *data = DecodeSample(encodedData, sampleHeader.Size, m_header.EncodingCode);
    delete[] encodedData;

    return read;
}

Gx::Int64 M30Archive::GetFile(const std::string& name, Gx::Uint8** data) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == name)
            return GetFile(key, data);
    }

    return -1;
}

Gx::Int64 M30Archive::GetFile(const FileEntry* entry, Gx::Uint8** data) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == entry->Name)
            return GetFile(key, data);
    }

    return -1;
}

std::vector<Gx::Archive::FileEntry> M30Archive::GetFileEntries() const
{
    std::vector<FileEntry> result;

    m_entries.clear();
    m_fileStream.seek(m_header.SampleOffset);
    for (unsigned int i = 0; i < m_header.SampleCount; i++)
    {
        auto entry = O2FileEntry();
        auto offset = m_fileStream.tell();
        if (offset == -1)
            continue;

        auto sampleHeader = M30SampleHeader();
        if (!Read(&sampleHeader, sizeof(sampleHeader)))
            continue;

        if (m_fileStream.seek(m_fileStream.tell() + sampleHeader.Size) == -1)
            continue;

        entry.Parent = this;
        entry.Name   = std::string(sampleHeader.Name, sizeof(sampleHeader.Name)).c_str();
        entry.Size   = sampleHeader.Size;
        entry.Offset = offset;

        m_entries[i] = entry;
        result.push_back(entry);
    }

    return result;
}

bool M30Archive::Read(void* data, Gx::Uint64 size) const
{
    auto read = m_fileStream.read(data, size);
    return read == size;
}

Gx::Uint8* M30Archive::DecodeSample(Gx::Uint8* encoded, int length, int encodingCode)
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