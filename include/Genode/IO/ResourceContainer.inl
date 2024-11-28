#pragma once

namespace Gx
{
    template<typename R>
    ResourceContainer<R>::ResourceContainer() :
        m_caches()
    {
    }

    template<typename R>
    ResourceContainer<R>::~ResourceContainer()
    {
    }

    template<typename R>
    R& ResourceContainer<R>::Store(const std::string& id, ResourcePtr<R> resource, const CacheMode mode)
    {
        auto current = Find(id);
        if (current)
        {
            if (mode == CacheMode::None)
                throw ResourceStoreException(id, "[" + id + "]\nResource with same ID is already exists");

            if (mode == CacheMode::Reuse)
                return *current;
        }

        if (!resource)
            throw ResourceStoreException(id, "[" + id + "]\nCannot store empty resource");

        m_caches[id] = std::move(resource);
        return *m_caches[id];
    }

    template<typename R>
    R& ResourceContainer<R>::Store(const std::string& id, std::function<ResourcePtr<R>()> deserializer, const CacheMode mode)
    {
        auto current = Find(id);
        if (current)
        {
            if (mode == CacheMode::None)
                throw ResourceStoreException(id, "[" + id + "]\nResource with same ID is already exists");

            if (mode == CacheMode::Reuse)
                return *current;
        }

        auto resource = deserializer();
        if (!resource)
            throw ResourceStoreException(id, "[" + id + "]\nCannot store empty resource.");

        m_caches[id] = std::move(resource);
        return *m_caches[id];
    }

    template<typename R>
    bool ResourceContainer<R>::Destroy(R* resource)
    {
        if (!resource)
            return false;

        auto it = std::find_if(m_caches.begin(), m_caches.end(), [resource] (const auto it) {
            return resource == it->second.get();
        });

        if (it != m_caches.end())
            return m_caches.erase(it) != 0;

        return false;
    }

    template<typename R>
    R* ResourceContainer<R>::Find(const std::string& id) const
    {
        auto iterator = m_caches.find(id);
        if (iterator != m_caches.end())
            return iterator->second.get();

        return nullptr;
    }

    template<typename R>
    R& ResourceContainer<R>::Get(const std::string& id) const
    {
        assert(m_caches.contains(id));

        return *m_caches[id];
    }

    template<typename R>
    void ResourceContainer<R>::Each(const std::function<void(const std::string& , R &)> &callback)
    {
        if (!callback)
            return;

        for (auto& [key, resource] : m_caches)
            callback(key, *resource.get());
    }

    template<typename R>
    bool ResourceContainer<R>::Contains(const std::string& id) const
    {
        return m_caches.contains(id);
    }

    template<typename R>
    std::uint64_t ResourceContainer<R>::Count() const
    {
        return m_caches.size();
    }

    template<typename R>
    bool ResourceContainer<R>::Destroy(const std::string& id)
    {
        return m_caches.erase(id) != 0;
    }

    template<typename R>
    void ResourceContainer<R>::Clear()
    {
        m_caches.clear();
    }
}