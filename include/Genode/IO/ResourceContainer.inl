#pragma once

#include <Genode/Utilities/StringHelper.hpp>

namespace Gx
{
    template<typename R, typename U>
    ResourceContainer<R, U>::ResourceContainer() :
        m_caches()
    {
    }

    template<typename R, typename U>
    ResourceContainer<R, U>::~ResourceContainer()
    {
    }

    template<typename R, typename U>
    R& ResourceContainer<R, U>::Store(const U& id, ResourcePtr<R> resource, const CacheMode mode)
    {
        auto current = Find(id);
        if (current)
        {
            if (mode == CacheMode::None)
                throw ResourceStoreException(StringHelper::ToString(id), "[" + StringHelper::ToString(id) + "]\nResource with same ID is already exists");

            if (mode == CacheMode::Reuse)
                return *current;
        }

        if (!resource)
            throw ResourceStoreException(StringHelper::ToString(id), "[" + StringHelper::ToString(id) + "]\nCannot store empty resource");

        m_caches[id] = std::move(resource);
        return *m_caches[id];
    }

    template<typename R, typename U>
    R& ResourceContainer<R, U>::Store(const U& id, std::function<ResourcePtr<R>()> deserializer, const CacheMode mode)
    {
        auto current = Find(id);
        if (current)
        {
            if (mode == CacheMode::None)
                throw ResourceStoreException(StringHelper::ToString(id), "Resource with same resource id (" + StringHelper::ToString(id) + ") is already exists");

            if (mode == CacheMode::Reuse)
                return *current;
        }

        auto resource = deserializer();
        if (!resource)
            throw ResourceStoreException(StringHelper::ToString(id), "Cannot store empty resource");

        m_caches[id] = std::move(resource);
        return *m_caches[id];
    }

    template<typename R, typename U>
    bool ResourceContainer<R, U>::Destroy(R* resource)
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

    template<typename R, typename U>
    R* ResourceContainer<R, U>::Find(const U& id) const
    {
        auto iterator = m_caches.find(id);
        if (iterator != m_caches.end())
            return iterator->second.get();

        return nullptr;
    }

    template<typename R, typename U>
    R& ResourceContainer<R, U>::Get(const U& id) const
    {
        if (!m_caches.contains(id))
            throw ResourceAccessException(StringHelper::ToString(id));

        return *m_caches[id];
    }

    template<typename R, typename U>
    void ResourceContainer<R, U>::Each(const std::function<void(const U& , R &)> &callback)
    {
        if (!callback)
            return;

        for (auto& [key, resource] : m_caches)
            callback(key, *resource.get());
    }

    template<typename R, typename U>
    bool ResourceContainer<R, U>::Contains(const U& id) const
    {
        return m_caches.contains(id);
    }

    template<typename R, typename U>
    std::uint64_t ResourceContainer<R, U>::Count() const
    {
        return m_caches.size();
    }

    template<typename R, typename U>
    bool ResourceContainer<R, U>::Destroy(const U& id)
    {
        return m_caches.erase(id) != 0;
    }

    template<typename R, typename U>
    void ResourceContainer<R, U>::Clear()
    {
        m_caches.clear();
    }
}