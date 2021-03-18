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
        static ResourceManager instance;
        return &instance;
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