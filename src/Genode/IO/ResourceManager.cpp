#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/CacheManager.hpp>

namespace Gx
{
    ResourceManager::ResourceManager() :
        m_cache(CacheManager::Instance()),
        m_archives(),
        m_entries()
    {
    }
    
    ResourceManager::~ResourceManager()
    {
        m_cache->Clear();
        for (auto entry : m_entries)
        {
            if (entry.second)
                delete entry.second;
        }

        for (auto archive : m_archives)
        {
            if (archive.second)
                delete archive.second;
        }
    }

    ResourceManager* ResourceManager::Instance()
    {
        static ResourceManager instance;
        return &instance;
    }
}