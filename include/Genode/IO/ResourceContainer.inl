namespace Gx
{
    template<typename R>
    inline ResourceContainer<R>::ResourceContainer() :
        m_caches()
    {
    }

    template<typename R>
    inline ResourceContainer<R>::~ResourceContainer()
    {
        m_caches.clear();
    }

    template<typename R>
    inline R* ResourceContainer<R>::Add(const std::string& name, ResourcePtr<R> resource, bool useCache)
    {
        if (useCache)
        {
            if (auto cache = Find(name))
                return cache;
        }

        if (!resource)
            return nullptr;

        m_caches[name] = std::move(resource);
        return m_caches[name].get();
    }

    template<typename R>
    R* ResourceContainer<R>::Add(const std::string &name, std::function<ResourcePtr<R>()> resolver, bool useCache)
    {
        if (useCache)
        {
            if (auto cache = Find(name))
                return cache;
        }

        auto resource = resolver();
        if (!resource)
            return nullptr;

        m_caches[name] = std::move(resource);
        return m_caches[name].get();
    }

    template<typename R>
    bool ResourceContainer<R>::Remove(R *resource)
    {
        auto it = std::find_if(m_caches.begin(), m_caches.end(), [resource] (const auto it) { return resource == it->second.get(); });
        if (it != m_caches.end())
            return m_caches.erase(it) != 0;

        return false;
    }

    template<typename R>
    inline R* ResourceContainer<R>::Find(const std::string& name) const
    {
        auto iterator = m_caches.find(name);
        if (iterator != m_caches.end())
            return iterator->second.get();

        return nullptr;
    }

    template<typename R>
    inline R& ResourceContainer<R>::Get(const std::string& name) const
    {
        assert(m_caches.find(name) != m_caches.end());

        return *m_caches[name];
    }

    template<typename R>
    inline bool ResourceContainer<R>::Contains(const std::string& identifier) const
    {
        return m_caches.find(identifier) != m_caches.end();
    }

    template<typename R>
    inline Uint64 ResourceContainer<R>::Count() const
    {
        return m_caches.size();
    }

    template<typename R>
    inline bool ResourceContainer<R>::Remove(const std::string& identifier)
    {
        return m_caches.erase(identifier) != 0;
    }

    template<typename R>
    inline void ResourceContainer<R>::Clear()
    {
        m_caches.clear();
    }
}