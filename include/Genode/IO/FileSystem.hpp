#ifndef GENODE_FILESYSTEM_HPP
#define GENODE_FILESYSTEM_HPP

#include <Genode/System/Primitives.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <vector>
#include <string>

namespace Gx
{
    class FileSystem
    {
    public:
        static FileSystem* Instance();

        FileSystem();
        virtual ~FileSystem();

        std::vector<std::string> GetPaths();
        void AddPath(const std::string& path);

        bool Exists(const std::string& fileName) const;
        std::string GetFileName(const std::string& fullPath) const;
        std::string GetFullName(const std::string& fileName) const;

        Int64 ReadFile(const std::string& filename, Uint8** data) const;

    private:
        std::vector<std::string> m_directories;
    };
}

#endif