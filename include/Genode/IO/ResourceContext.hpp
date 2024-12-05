#pragma once

#include <Genode/IO/ResourceContainer.hpp>
#include <SFML/System/InputStream.hpp>
#include <string>

namespace Gx
{
    class ResourceManager;
    class ResourceContext
    {
    public:
        static const ResourceContext Default;

        explicit ResourceContext(const std::string& id);
        ResourceContext(const std::string& id, ResourceManager& resources, CacheMode mode = CacheMode::None);

        virtual ~ResourceContext() = default;

        template<typename Ctx>
        static std::enable_if_t<std::is_base_of_v<ResourceContext, Ctx>, Ctx>
        Rebind(const Ctx& ctx, const std::string& id);

        template<typename Ctx>
        std::enable_if_t<std::is_base_of_v<ResourceContext, Ctx>, Ctx>
        Rebind(const Ctx& ctx, ResourceManager& resources);

        const std::string& GetID() const;
        bool Available() const;

        template<typename R>
        R* Find(const std::string& id) const;

        template<typename R>
        R& Acquire(const std::string& id) const;

        template<typename R>
        R& Acquire(const std::string& id, const std::string& path) const;

        template<typename R>
        R& Acquire(const std::string& id, const void* data, std::size_t dataSize) const;

        template<typename R>
        R& Acquire(const std::string& id, sf::InputStream& stream) const;

        template<typename R>
        R& Store(const std::string& id, R& resource) const;

        template<typename R>
        R& Store(const std::string& id, ResourcePtr<R> resource) const;

        CacheMode GetCacheMode() const;

        void Bind(ResourceManager& resources);

        void Unbind();

    protected:
        ResourceContext();

        ResourceManager* GetResourceManager() const;

    private:
        ResourceContext(const std::string& id, ResourceManager* resources, CacheMode mode = CacheMode::None);

        std::string m_id;
        CacheMode m_cacheMode = CacheMode::None;
        ResourceManager* m_resources;
    };
}

#include <Genode/IO/ResourceContext.inl>
