#include <OTwo/Archives/OjmArchive.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <string>

bool OjmArchive::LoadFromFile(const std::string &fileName)
{
    auto fs = Gx::FileSystem::Open(fileName);
    char sign[3];

    if (fs->read(&sign[0], 3) != 3)
        return false;

    delete fs.release(); // Force releasing file handle

    auto signature = std::string(sign, 3);
    if (signature == "M30")
    {
        m_type = ArchiveType::M30;
        return M30Archive::LoadFromFile(fileName);
    }
    else if (signature == "OMC" || signature == "OJM")
    {
        m_type = ArchiveType::OMC;
        return OmcArchive::LoadFromFile(fileName);
    }

    return false;
}

Gx::ResourcePtr<sf::InputStream> OjmArchive::Open(unsigned int index) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::Open(index);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::Open(index);

    return nullptr;
}


Gx::ResourcePtr<sf::InputStream> OjmArchive::Open(const std::string &fileName) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::Open(fileName);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::Open(fileName);

    return nullptr;
}

bool OjmArchive::Contains(const std::string &name) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::Contains(name);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::Contains(name);

    return false;
}

std::vector<Gx::FileInfo> OjmArchive::GetFileEntries() const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::GetFileEntries();
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::GetFileEntries();

    return {};
}

std::unique_ptr<Gx::FileInfo> OjmArchive::GetFileInfo(const std::string &fileName) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::GetFileInfo(fileName);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::GetFileInfo(fileName);

    return {};
}

Gx::Int64 OjmArchive::ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::ReadFile(fileName, data, size);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::ReadFile(fileName, data, size);

    return -1;
}

Gx::Int64 OjmArchive::GetFileSize(const std::string &fileName) const
{
    if (m_type == ArchiveType::M30)
        return M30Archive::GetFileSize(fileName);
    else if (m_type == ArchiveType::OMC)
        return OmcArchive::GetFileSize(fileName);

    return -1;
}
