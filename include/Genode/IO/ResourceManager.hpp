#ifndef GENODE_RESOURCE_MANAGER_HPP
#define GENODE_RESOURCE_MANAGER_HPP

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>

#include <unordered_map>
#include <string>

namespace Gx
{
    class ResourceContainer;
    class ResourceManager
    {
    public:
        static ResourceManager* Instance();

		ResourceManager();
		virtual ~ResourceManager();

        template<class T>
        T* AddArchive(const std::string& filename);

		template<class T>
		T* GetArchive(const std::string& filename) const;

		template<class T>
		T* Instantiate(const std::string& name, bool cache = true);

    private:
        ResourceContainer* m_resources;
		std::unordered_map<std::string, Archive*> m_archives;
        std::unordered_map<std::string, const Archive::FileEntry*> m_entries;
    };
}

#include <Genode/IO/ResourceManager.inl>
#endif