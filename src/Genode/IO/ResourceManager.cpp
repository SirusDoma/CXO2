#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/CacheManager.hpp>

namespace Gx
{
    ResourceManager::ResourceManager(CacheManager *cacheManager) :
        m_cache(cacheManager),
        m_archives(),
        m_entries()
    {

    }
    
    ResourceManager::~ResourceManager()
    {
        m_cache->Clear();
        for (auto archive : m_archives)
        {
            if (archive.second)
            {
                delete archive.second;
                archive.second = nullptr;
            }
        }
    }

    ResourceManager* ResourceManager::Instance()
    {
        static ResourceManager instance(new CacheManager());
        return &instance;
    }

    ResourceContext ResourceManager::GetResourceContext(ResourceMetadata *metadata)
    {
        // Load required resources to build resource from metadata
        auto context = ResourceContext();
        for (auto resource : metadata->ResourceReferences)
        {
            if (resource.first == "texture")
                context.Texture = GetResource<sf::Texture>(resource.second);
            else if (resource.first == "font")
                context.Font    = GetResource<sf::Font>(resource.second);
        }

        return context;
    }

    Uint64 ResourceManager::GetResourceData(const std::string &name, Gx::Uint8 **data)
    {
        auto iterator = m_entries.find(name);
        if (iterator != m_entries.end())
        {
            auto entry = iterator->second;
            return entry.GetContent(data);
        }
        else if (FileHelper::Exists(name, true))
            return FileHelper::GetFile(name, data);
        else
            return 0;
    }
}