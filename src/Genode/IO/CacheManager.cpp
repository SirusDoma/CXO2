#include <Genode/IO/CacheManager.hpp>

namespace Gx
{
    CacheManager::CacheManager() :
        m_cacheMap()
    {
    }

    CacheManager::~CacheManager()
    {
        // Shared ptr will be deleted upon handle released from this handle and goes out of scope
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
        auto iterator = m_cacheMap.find(identifier);
        if (iterator != m_cacheMap.end())
        {
            m_cacheMap.erase(iterator);
            return true;
        }

        return false;
    }
}