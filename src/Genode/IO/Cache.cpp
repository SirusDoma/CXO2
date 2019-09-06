#include <Genode/IO/Cache.hpp>

namespace Gx
{
    Cache::Cache() :
        m_caches()
    {
    }

    Cache::~Cache()
    {
		// Shared ptr will be deleted upon handle released from this handle and goes out of scope
        m_caches.clear();
    }

    Cache* Cache::Instance()
    {
        static Cache instance = Cache();
        return &instance;
    }

    bool Cache::Remove(const std::string& identifier)
    {
        auto iterator = m_caches.find(identifier);
        if (iterator != m_caches.end())
        {
            m_caches.erase(iterator);
            return true;
        }

        return false;
    }
}