#include <Genode/IO/CacheManager.hpp>

namespace Gx
{
    CacheManager::CacheManager() :
        m_cacheMap()
    {
    }

    CacheManager::~CacheManager()
    {
    }

    CacheManager* CacheManager::Instance()
    {
        static CacheManager instance = CacheManager();
        return &instance;
    }

    bool CacheManager::Contains(const std::string& identifier) const
    {
        auto iterator = m_cacheMap.find(identifier);
        return iterator != m_cacheMap.end();
    }

    Uint64 CacheManager::Count() const
    {
        return m_cacheMap.size();
    }

    bool CacheManager::Remove(const std::string& identifier)
    {
        return m_cacheMap.erase(identifier) != 0;
    }
    
    void CacheManager::Clear()
    {
        m_cacheMap.clear();
    }
}