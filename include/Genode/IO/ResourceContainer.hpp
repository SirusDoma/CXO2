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

    template<typename R>
    class ResourceContainer final
    {
    public:
        ResourceContainer();
        ResourceContainer(const ResourceContainer&) = delete;
        ResourceContainer& operator=(const ResourceContainer&) = delete;

        ~ResourceContainer();

        R& Store(const std::string& id, ResourcePtr<R> resource, CacheMode mode = CacheMode::Reuse);
        R& Store(const std::string& id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode = CacheMode::Reuse);

        bool Destroy(R* resource);
        bool Destroy(const std::string& id);

        R* Find(const std::string& id) const;
        R& Get(const std::string& id) const;
        void Each(const std::function<void(const std::string&, R&)> &callback);

        [[nodiscard]] bool Contains(const std::string& id) const;
        [[nodiscard]] std::uint64_t Count() const;
        void Clear();

    private:
        using ResourceMap = std::unordered_map<std::string, ResourcePtr<R>>;

        ResourceMap m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
