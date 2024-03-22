#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO/FileSystem/FileInfo.hpp>

#include <algorithm>
#include <iostream>

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
    LocalFileSystem &LocalFileSystem::Instance()
    {
        static ResourcePtr<LocalFileSystem> instance;
        if (!instance)
            instance = ResourcePtr<LocalFileSystem>(new LocalFileSystem(), [] (auto ptr) { delete ptr; });

        return *instance.get();
    }

    void LocalFileSystem::SetWorkingDirectory(const std::string &inputPath)
    {
        auto workingDir = path(inputPath);
        if (workingDir.has_filename() || is_directory(workingDir))
            workingDir = workingDir.parent_path();

        if (!exists(workingDir))
            return;

        std::filesystem::current_path(workingDir);
    }

    std::vector<std::string> LocalFileSystem::GetAssetPaths()
    {
        return m_paths;
    }

    void LocalFileSystem::AddAssetPath(const std::string& path)
    {
        m_paths.push_back(path);
    }

    ResourcePtr<sf::InputStream> LocalFileSystem::Open(const std::string &fileName) const
    {
        const auto fileStream = new sf::FileInputStream();
        auto stream = ResourcePtr<sf::InputStream>(fileStream, [] (auto fs) { delete fs; });
        if (fileStream->open(GetFullName(fileName)))
            return stream;

        return nullptr;
    }

    std::unique_ptr<FileInfo> LocalFileSystem::GetFileInfo(const std::string &fileName) const
    {
        const auto fullName = GetFullName(fileName);
        auto size = GetFileSize(fullName);

        return std::make_unique<FileInfo>(*this, fullName, size);
    }

    Int64 LocalFileSystem::GetFileSize(const std::string &fileName) const
    {
        Int64 size = -1;
        if (auto fileStream = sf::FileInputStream(); fileStream.open(GetFullName(fileName)))
            size = fileStream.getSize();

        return size;
    }

    bool LocalFileSystem::Contains(const std::string& fileName) const
    {
        if (fileName.empty())
            return false;

        if (const auto filePath = path(fileName.c_str()); exists(filePath))
            return true;

        for (std::string& path : m_paths)
        {
            std::string fullPath = std::string(path).append("/").append(fileName);
            if (exists(fullPath.c_str()))
                return true;
        }

        return false;
    }

    std::string LocalFileSystem::GetFileName(const std::string& fullPath, const bool withExtension) const
    {
        const auto filePath = path(fullPath.c_str());
        if (withExtension)
            return filePath.filename().string();

        return filePath.filename().replace_extension().string();
    }

    std::string LocalFileSystem::GetFullName(const std::string& fileName, const bool withExtension) const
    {
        for (std::string &dir : m_paths)
        {
            if (std::string fullPath = std::string(dir).append("/").append(fileName); exists(fullPath))
            {
                if (withExtension)
                    return fullPath;

                return path(fullPath).replace_extension().string();
            }
        }

        return "";
    }

    Int64 LocalFileSystem::ReadFile(const std::string& fileName, void* data, Int64 size) const
    {
        sf::FileInputStream fs;
        fs.open(GetFullName(fileName));

        if (size < 0)
            size = fs.getSize();

        if (size <= 0)
            return size;

        return fs.read(data, size);
    }

    void LocalFileSystem::WriteFile(const std::string &fileName, void *data, Int64 size) const
    {
        if (size <= 0)
            return;

        std::ofstream fs(fileName.c_str(), std::ios::out | std::ios::binary);
        fs.write(reinterpret_cast<const char*>(data), size);

        fs.close();
    }
}
