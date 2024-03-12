#ifndef GENODE_IO_CACHE_HPP
#define GENODE_IO_CACHE_HPP

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/IOException.hpp>

#include <Genode/System/Primitives.hpp>
#include <Genode/System/NonCopyable.hpp>

#include <functional>
#include <unordered_map>
#include <string>

namespace Gx
{
    enum CacheMode
    {
        None,
        Update,
        Reuse
    };

    template<typename R>
    class ResourceContainer final : NonCopyable
    {
    public:
        ResourceContainer();
        ~ResourceContainer();

        R& Store(const std::string& id, ResourcePtr<R> resource, CacheMode mode = CacheMode::Reuse);
        R& Store(const std::string& id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode = CacheMode::Reuse);

        bool Destroy(R* resource);
        bool Destroy(const std::string& id);

        R* Find(const std::string& id) const;
        R& Get(const std::string& id) const;

        bool   Contains(const std::string& id) const;
        Uint64 Count() const;

        void   Clear();

    private:
        using ResourceMap = std::unordered_map<std::string, ResourcePtr<R>>;

        ResourceMap m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
#endif