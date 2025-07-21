#include <Genode/Utilities/StringHelper.hpp>
#include <OTwo/Archives/OpiArchive.hpp>
#include <SFML/System/MemoryInputStream.hpp>

#pragma warning(disable: 6054)

OpiArchive::Signature OpiArchive::GetSignature() const
{
    return m_signature;
}

bool OpiArchive::LoadFromFile(const std::string& fileName)
{
    // Fetch meta data
    m_source = fileName;
    if (!m_fileStream.open(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
        return false;

    const auto prefix = Gx::StringHelper::RemoveExtension(fileName);
    SetPathPrefix(prefix + "/");

    if (!m_fileStream.seek(0).has_value())
        return false;

    if (!ReadStream(&m_signature, sizeof(m_signature)))
        return false;

    if (m_signature != Signature::OPI && m_signature != Signature::OPA)
        return false;

    if (!ReadStream(&m_count, sizeof(m_count)))
        return false;

    const auto entries = GetFileEntries();
    return !entries.empty(); // m_count == entries.size();
}

Gx::ResourcePtr<sf::InputStream> OpiArchive::Open(const std::string& fileName) const
{
    const auto it = m_entries.find(fileName);
    if (it == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified index is out of bound for this archive");

    const auto header = it->second;
    const auto data = new std::uint8_t[header.GetSize()];
    if (!ReadFile(fileName, data, header.GetSize()).has_value())
        delete[] data;

    return Gx::ResourcePtr<sf::InputStream>(new sf::MemoryInputStream(data, header.GetSize()), [data] (const sf::InputStream* ms)
    {
        delete[] data;
        delete ms;
    });
}

bool OpiArchive::Contains(const std::string& name) const
{
    const auto iterator = m_entries.find(name);
    return iterator != m_entries.end();
}

std::vector<std::unique_ptr<Gx::FileInfo>> OpiArchive::GetFileEntries() const
{
    std::vector<std::unique_ptr<Gx::FileInfo>> result;

    // Go to first item header offset
    if (const auto offset = m_count * ITEM_HEADER_SIZE; !m_fileStream.seek(m_fileStream.getSize().value_or(0) - offset).has_value())
        return result;

    // Traverse the header
    m_entries.clear();
    for (unsigned int i = 0; i < m_count; i++)
    {
        std::uint32_t sign;
        if (!ReadStream(&sign, sizeof(sign)) && sign != 01)
            continue;

        auto item = OpiItemHeader();
        if (!ReadStream(&item, sizeof(OpiItemHeader)))
            continue;

        auto header = FileInfo(*this, Gx::StringHelper::Trim(item.Name), item.GetSize(), i, item.Reference);
        m_entries[header.GetName()] = header;

        result.push_back(std::make_unique<FileInfo>(header));
    }

    return result;
}

std::unique_ptr<Gx::FileInfo> OpiArchive::GetFileInfo(const std::string& fileName) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.GetName() == fileName)
            return std::make_unique<FileInfo>(header);
    }

    throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");
}

std::optional<std::size_t> OpiArchive::ReadFile(const std::string& fileName, void* data, std::size_t size) const
{
    const auto iterator = m_entries.find(fileName);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");

    const FileInfo header = iterator->second;
    if (size > header.GetSize())
        size = header.GetSize();

    {
        auto lock = std::lock_guard(m_mutex);
        if (!m_fileStream.seek(static_cast<std::int64_t>(header.GetOffset())).has_value())
            throw Gx::ResourceAccessException(fileName, "Failed to seek the data inside the archive");

        return m_fileStream.read(data, size);
    }
}

std::optional<std::size_t> OpiArchive::GetFileSize(const std::string& fileName) const
{
    const auto iterator = m_entries.find(fileName);
    if (iterator == m_entries.end())
        throw Gx::ResourceAccessException(fileName, "The specified name is not found for this archive");

    return iterator->second.GetSize();
}

bool OpiArchive::ReadStream(void* data, std::uint64_t size) const
{
    const auto read = m_fileStream.read(data, static_cast<std::int64_t>(size));
    return read == size;
}
