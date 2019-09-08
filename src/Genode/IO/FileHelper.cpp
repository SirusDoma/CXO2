#include <Genode/IO/FileHelper.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <memory>
#include <algorithm>
#include <filesystem>

namespace Gx
{
    std::vector<std::string> FileHelper::m_directories;

    std::vector<std::string> FileHelper::GetAssetPaths()
    {
        return m_directories;
    }

    void FileHelper::AddAssetPath(const std::string& path)
    {
        m_directories.push_back(path);
    }

    bool FileHelper::Exists(const std::string& fileName)
    {
        auto path = std::filesystem::path(fileName.c_str());
        return std::filesystem::exists(path);
    }

    std::string FileHelper::GetFileName(const std::string& filename)
    {
        auto path = std::filesystem::path(filename.c_str());
        return path.filename().string();
    }

    std::string FileHelper::GetFullName(const std::string& fileName)
    {
        if (Exists(fileName))
            return fileName;

        for (auto path : m_directories)
        {
            std::string fullPath = path + "/" + fileName;
            if (Exists(fullPath))
                return fullPath;
        }

        return "";
    }

    Int64 FileHelper::GetFile(const std::string& filename, Uint8** data)
    {
        sf::FileInputStream fs;
        fs.open(GetFullName(filename));

        auto size = fs.getSize();
        if (size <= 0)
            return size;

        *data = new Uint8[static_cast<unsigned int>(size)];
        return fs.read((char*) & (*data)[0], fs.getSize());
    }
}