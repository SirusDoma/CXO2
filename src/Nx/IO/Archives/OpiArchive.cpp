#include <Nx/IO/Archives/OpiArchive.hpp>

#pragma warning(disable: 6054)

OpiArchive::OpiArchive() :
        m_fileStream(),
        m_entries(),
        m_signature(),
        m_count()
{
}

OpiArchive::~OpiArchive()
{
}

OpiArchive::Signature OpiArchive::GetSignature() const
{
    return m_signature;
}

bool OpiArchive::Open(const std::string& fileName)
{
    if (!Archive::Open(fileName))
        return false;

    // Fetch meta data
    m_fileStream.open(fileName);
    m_fileStream.seek(0);

    if (!Read(&m_signature, sizeof(m_signature)))
        return false;

    if (m_signature != Signature::OPI && m_signature != Signature::OPA)
        return false;

    return Read(&m_count, sizeof(m_count)) != 0;
}

bool OpiArchive::Contains(const std::string& name) const
{
    auto iterator = m_entries.find(name);
    return iterator != m_entries.end();
}

Gx::Int64 OpiArchive::GetFile(const std::string& name, Gx::Uint8** data) const
{
    auto iterator = m_entries.find(name);
    if (iterator == m_entries.end())
        return -1;

    const O2FileEntry* header = &iterator->second;
    if (m_fileStream.seek(header->Offset) < 0)
        return -1;

    *data = new Gx::Uint8[header->Size];
    return m_fileStream.read((char*) & (*data)[0], header->Size);
}

Gx::Int64 OpiArchive::GetFile(const Archive::FileEntry* entry, Gx::Uint8** data) const
{
    auto header = dynamic_cast<const O2FileEntry*>(entry);
    if (!header)
        return -1;

    if (m_fileStream.seek(header->Offset) < 0)
        return -1;

    *data = new Gx::Uint8[header->Size];
    return m_fileStream.read((char*) & (*data)[0], header->Size);
}

std::vector<Gx::Archive::FileEntry> OpiArchive::GetFileEntries()
{
    // Go to first item header offset
    auto offset = m_count * ITEM_HEADER_SIZE;
    m_fileStream.seek(m_fileStream.getSize() - offset);

    // Traverse the header
    std::vector<FileEntry> result;
    m_entries.clear();
    for (unsigned int i = 0; i < m_count; i++)
    {
        O2FileEntry header;
        Gx::Uint32 sign;
        if (!Read(&sign, sizeof(sign)) && sign != 01)
            continue;

        char bytes[128];
        if (!Read(&bytes, sizeof(bytes)))
            continue;

        Gx::Uint32 ref;
        if (!Read(&ref, sizeof(ref)))
            continue;

        Gx::Uint32 size1;
        if (!Read(&size1, sizeof(size1)))
            continue;

        Gx::Uint32 size2;
        if (!Read(&size2, sizeof(size2)))
            continue;

        header.Parent = this;
        header.Name   = std::string(bytes);
        header.Offset = ref;
        header.Size   = size1 > size2 ? size1 : size2;

        m_entries[header.Name] = header;
        result.push_back(header);
    }

    return result;
}

Gx::Uint64 OpiArchive::Read(void* data, Gx::Uint64 size) const
{
    auto read = m_fileStream.read(data, size);
    return read == size;
}
