#include <OTwo/Archives/OpiArchive.hpp>
#include <SFML/System/MemoryInputStream.hpp>

#pragma warning(disable: 6054)

OpiArchive::Signature OpiArchive::GetSignature() const
{
    return m_signature;
}

bool OpiArchive::LoadFromFile(const std::string& fileName)
{
    if (!Archive::LoadFromFile(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
        return false;

    // Fetch meta data
    m_fileStream.open(Gx::LocalFileSystem::Instance().GetFullName(fileName));
    m_fileStream.seek(0);

    if (!ReadStream(&m_signature, sizeof(m_signature)))
        return false;

    if (m_signature != Signature::OPI && m_signature != Signature::OPA)
        return false;

    return ReadStream(&m_count, sizeof(m_count));
}

Gx::ResourcePtr<sf::InputStream> OpiArchive::Open(const std::string &fileName) const
{
    const auto it = m_entries.find(fileName);
    if (it == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified index is out of bound for this archive.");

    const auto header = it->second;
    const auto data = new Gx::Uint8[header.GetSize()];
    if (const int read = ReadFile(fileName, data, header.GetSize()); read <= 0)
        delete[] data;

    const auto stream = new sf::MemoryInputStream();
    stream->open(data, header.GetSize());

    return Gx::ResourcePtr<sf::InputStream>(stream, [data] (const sf::InputStream *ms) {
        delete ms;
        delete[] data;
    });
}

bool OpiArchive::Contains(const std::string& name) const
{
    const auto iterator = m_entries.find(name);
    return iterator != m_entries.end();
}

std::vector<Gx::FileInfo> OpiArchive::GetFileEntries() const
{
    // Go to first item header offset
    const auto offset = m_count * ITEM_HEADER_SIZE;
    m_fileStream.seek(m_fileStream.getSize() - offset);

    // Traverse the header
    std::vector<Gx::FileInfo> result;
    m_entries.clear();
    for (unsigned int i = 0; i < m_count; i++)
    {
        Gx::Uint32 sign;
        if (!ReadStream(&sign, sizeof(sign)) && sign != 01)
            continue;

        char bytes[128];
        if (!ReadStream(&bytes, sizeof(bytes)))
            continue;

        Gx::Uint32 ref;
        if (!ReadStream(&ref, sizeof(ref)))
            continue;

        Gx::Uint32 size1;
        if (!ReadStream(&size1, sizeof(size1)))
            continue;

        Gx::Uint32 size2;
        if (!ReadStream(&size2, sizeof(size2)))
            continue;

        auto header = FileInfo(
            *this,
            std::string(bytes),
            ref,
            size1 > size2 ? size1 : size2
        );

        m_entries[header.GetName()] = header;
        result.push_back(header);
    }

    return result;
}

std::unique_ptr<Gx::FileInfo> OpiArchive::GetFileInfo(const std::string &fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return std::make_unique<FileInfo>(header);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");
}

Gx::Int64 OpiArchive::ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const
{
    const auto iterator = m_entries.find(fileName);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");

    const FileInfo header = iterator->second;
    if (m_fileStream.seek(header.GetOffset()) < 0)
        throw Gx::ResourceAccessException(fileName, "Failed to seek the data inside the archive.");

    if (size > header.GetSize())
        size = header.GetSize();

    return m_fileStream.read(data, size);
}

Gx::Int64 OpiArchive::GetFileSize(const std::string &fileName) const
{
    const auto iterator = m_entries.find(fileName);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive.");

    return iterator->second.GetSize();
}

bool OpiArchive::ReadStream(void* data, Gx::Uint64 size) const
{
    const auto read = m_fileStream.read(data, size);
    return read == size;
}
