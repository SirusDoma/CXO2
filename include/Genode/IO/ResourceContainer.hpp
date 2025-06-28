#pragma once

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/IOException.hpp>

#include <functional>
#include <unordered_map>
#include <string>

namespace Gx
{
    enum class CacheMode
    {
        None,
        Update,
        Reuse
    };

    template<typename R, typename U = std::string>
    class ResourceContainer
    {
    public:
        ResourceContainer();
        ResourceContainer(const ResourceContainer&) = delete;
        ResourceContainer& operator=(const ResourceContainer&) = delete;

        ~ResourceContainer();

        R& Store(const U& id, ResourcePtr<R> resource, CacheMode mode = CacheMode::Reuse);
        R& Store(const U& id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode = CacheMode::Reuse);

        bool Destroy(R* resource);
        bool Destroy(const U& id);

        R* Find(const U& id) const;
        R& Get(const U& id) const;
        void Each(const std::function<void(const U&, R&)> &callback);

        [[nodiscard]] bool Contains(const U& id) const;
        [[nodiscard]] std::uint64_t Count() const;
        void Clear();

    private:
        using ResourceMap = std::unordered_map<U, ResourcePtr<R>>;

        ResourceMap m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
