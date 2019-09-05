#include <Nx/IO/OpiArchive.hpp>

#pragma warning(disable: 6054)

OpiArchive::OpiArchive() :
    m_fileStream(),
    m_headers(),
    m_signature()
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

    m_fileStream.open(fileName);

    // Fetch meta data
    m_fileStream.seek(0);
    if (!Read(&m_signature, sizeof(m_signature)))
        return false;

    if (m_signature != Signature::OPI && m_signature != Signature::OPA)
        return false;

    Gx::Uint32 count;
    if (!Read(&count, sizeof(count)))
        return false;

    // Go to first item header offset
    auto offset = count * ITEM_HEADER_SIZE;
    m_fileStream.seek(m_fileStream.getSize() - offset);

    // Traverse the header
    for (unsigned int i = 0; i < count; i++)
    {
        ItemHeader header;
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

        header.Identifier = std::string(bytes);
        header.Offset     = ref;
        header.Size       = size1 > size2 ? size1 : size2;

        m_headers[header.Identifier] = header;
    }

    return true;
}

bool OpiArchive::Contains(const std::string& name)
{
    auto iterator = m_headers.find(name);
    return iterator != m_headers.end();
}

Gx::Int64 OpiArchive::GetFile(const std::string& name, Gx::Uint8** data) const
{
    return Gx::Int64();
}

bool OpiArchive::Read(void* data, Gx::Uint64 size)
{
    auto read = m_fileStream.read(data, size);
    return read == size;
}