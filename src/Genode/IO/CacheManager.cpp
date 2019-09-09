#include <Genode/IO/CacheManager.hpp>

namespace Gx
{
    CacheManager::CacheManager() :
        m_cacheMap()
    {
    }

    CacheManager::~CacheManager()
    {
        m_cacheMap.clear();
    }

    CacheManager* CacheManager::Instance()
    {
        static CacheManager instance = CacheManager();
        return &instance;
    }

    bool CacheManager::Contains(const std::string& identifier)
    {
        auto iterator = m_cacheMap.find(identifier);
        return iterator != m_cacheMap.end();
    }

    bool CacheManager::Remove(const std::string& identifier)
    {
        return m_cacheMap.erase(identifier) != 0;
    }
}