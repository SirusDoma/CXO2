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

#include <fstream>

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

    bool FileHelper::Exists(const std::string& fileName, bool asAsset)
    {
        auto filePath = path(fileName.c_str());
        bool result = exists(filePath);
        if (!asAsset || result)
            return result;

        for (std::string& path : m_directories)
        {
            std::string fullPath = std::string(path).append("/").append(fileName);
            if (exists(fullPath.c_str()))
                return true;
        }

        return false;
    }

    std::string FileHelper::GetFileName(const std::string& filename, bool extension)
    {
        auto filePath = path(filename.c_str());

        if (extension)
            return filePath.filename().string();
        else
            return filePath.filename().replace_extension().string();
    }

    std::string FileHelper::GetFullName(const std::string& fileName, bool extension)
    {
        if (Exists(fileName))
            return fileName;

        for (std::string &filePath : m_directories)
        {
            std::string fullPath = std::string(filePath).append("/").append(fileName);
            if (Exists(fullPath))
            {
                if (extension)
                    return fullPath;
                else
                    return path(fullPath).replace_extension().string();
            }
        }

        return "";
    }

    Int64 FileHelper::GetFile(const std::string& fileName, Uint8** data)
    {
        sf::FileInputStream fs;
        fs.open(GetFullName(fileName));

        auto size = fs.getSize();
        if (size <= 0)
            return size;

        *data = new Uint8[static_cast<unsigned int>(size)];
        return fs.read((char*) & (*data)[0], fs.getSize());
    }

    void FileHelper::WriteFile(const std::string &fileName, Uint8 *data, Int64 size)
    {
        std::ofstream fs(fileName.c_str(), std::ios::out | std::ios::binary);
        fs.write(reinterpret_cast<const char*>(data), size);
        fs.close();
    }
}