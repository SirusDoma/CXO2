#include <Genode/IO/FileHelper.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <algorithm>

#if defined(__cpp_lib_filesystem)
#include <filesystem>
using namespace std::filesystem;
#else
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#endif

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
        auto filePath = path(fileName.c_str());
        return exists(filePath);
    }

    std::string FileHelper::GetFileName(const std::string& filename)
    {
        auto filePath = path(filename.c_str());
        return filePath.filename().string();
    }

    std::string FileHelper::GetFullName(const std::string& fileName)
    {
        if (Exists(fileName))
            return fileName;

        for (std::string &filePath : m_directories)
        {
            std::string fullPath = filePath.append("/").append(fileName);
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