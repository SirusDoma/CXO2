#include <Genode/IO/FileSystem.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <memory>
#include <algorithm>
#include <filesystem>

namespace
{
    struct MatchPathSeparator
    {
        bool operator()( char ch ) const
        {
            return ch == '\\' || ch == '/';
        }
    };
}

namespace Gx
{
    FileSystem::FileSystem() :
        m_directories()
    {
    }
    
    FileSystem::~FileSystem()
    {
        m_directories.clear();
    }

    FileSystem* FileSystem::Instance()
    {
        static auto instance = FileSystem();
        return &instance;
    }

    std::vector<std::string> FileSystem::GetPaths()
    {
        return m_directories;
    }

    void FileSystem::AddPath(const std::string& path)
    {
        m_directories.push_back(path);
    }

    bool FileSystem::Exists(const std::string& fileName) const
    {
        auto path = std::filesystem::path(fileName.c_str());
        return std::filesystem::exists(path);
    }

    std::string FileSystem::GetFileName(const std::string& filename) const
    {
        auto path = std::filesystem::path(filename.c_str());
        return path.filename().string();
    }

    std::string FileSystem::GetIdentifier(const std::string& filename) const
    {
        auto path = std::filesystem::path(filename.c_str());
        return path.replace_extension().string();
    }
    
    Int64 FileSystem::ReadFile(const std::string& filename, Uint8** data) const
    {
        sf::FileInputStream fs;
        if (m_directories.size() != 0)
        {
            for (auto path : m_directories)
            {
                if (fs.open(path + "/" + filename))
                    break;
            }
        }
        else
            fs.open(filename);

        auto size = fs.getSize();
        if (size <= 0)
            return size;

        *data = new Uint8[static_cast<unsigned int>(size)];
        return fs.read((char*)&(*data)[0], fs.getSize());
    }
}
