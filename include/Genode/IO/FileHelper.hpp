#ifndef GENODE_FILE_HELPER_HPP
#define GENODE_FILE_HELPER_HPP

#include <Genode/System/Primitives.hpp>

#include <vector>
#include <string>

namespace Gx
{
    class FileHelper
    {
    public:
        static std::vector<std::string> GetAssetPaths();
        static void AddAssetPath(const std::string& path);

        static std::string GetFileName(const std::string& fullPath);
        static std::string GetFullName(const std::string& fileName);

        static bool Exists(const std::string& fileName, bool asAsset = false);
        static Int64 GetFile(const std::string& fileName, Uint8** data);
        static void WriteFile(const std::string& fileName, Uint8* data, Int64 size);

    private:
        static std::vector<std::string> m_directories;
    };
}

#endif