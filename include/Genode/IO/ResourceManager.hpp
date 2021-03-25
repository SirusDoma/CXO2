#ifndef GENODE_RESOURCE_MANAGER_HPP
#define GENODE_RESOURCE_MANAGER_HPP

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileHelper.hpp>
#include <Genode/IO/MetadataLoader.hpp>
#include <Genode/IO/CacheManager.hpp>

#include <memory>
#include <unordered_map>
#include <string>

namespace Gx
{
    class ResourceManager
    {
    public:
        static ResourceManager* Instance();

        ResourceManager(CacheManager *cacheManager);
        virtual ~ResourceManager();

        template<typename T>
        T* AddArchive(const std::string& filename);

        template<typename T>
        T* GetArchive(const std::string& filename) const;

        template<typename T>
        ResourceMetadata* GetMetadata(const std::string& name, bool cache = true);

        template<typename T>
        T* Create(const std::string& name, bool cache = true);

        template<typename T>
        T* Create(ResourceMetadata* metadata);

        ResourceContext GetResourceContext(ResourceMetadata *metadata);
        Uint64 GetResourceData(const std::string& name, Gx::Uint8** data);

        CacheManager *GetCache() const;

    private:
        template<typename T>
        T* GetResource(const std::string name);

        CacheManager* m_cache;
        std::unordered_map<std::string, Archive*> m_archives;
        std::unordered_map<std::string, Archive::FileEntry> m_entries;
    };
}

#include <Genode/IO/ResourceManager.inl>
#endif