#ifndef GENODE_IO_RESOURCE_CONTEXT_HPP
#define GENODE_IO_RESOURCE_CONTEXT_HPP


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

        explicit ResourceContext(const std::string &id);
        ResourceContext(const std::string &id, ResourceManager &resources, CacheMode mode = CacheMode::None);

        virtual ~ResourceContext() = default;

        static ResourceContext Rebind(const std::string &id, const ResourceContext &ctx);

        const std::string &GetID() const;
        bool Available() const;

        template<typename R>
        R* Find(const std::string &id) const;

        template<typename R>
        R& Acquire(const std::string &id) const;

        template<typename R>
        R& Acquire(const std::string &id, const std::string &path) const;

        template<typename R>
        R& Acquire(const std::string &id, const void* data, std::size_t dataSize) const;

        template<typename R>
        R& Acquire(const std::string &id, sf::InputStream &stream) const;

        template<typename R>
        R& Store(const std::string &id, R& resource) const;

        template<typename R>
        R& Store(const std::string &id, ResourcePtr<R> resource) const;

        CacheMode GetCacheMode() const;

    protected:
        ResourceContext();

        ResourceManager *GetResourceManager() const;

    private:
        ResourceContext(const std::string &id, ResourceManager *resources, CacheMode mode = CacheMode::None);

        const std::string m_id;
        const CacheMode m_cacheMode = CacheMode::None;
        mutable ResourceManager *m_resources;
    };
}

#include <Genode/IO/ResourceContext.inl>
#endif