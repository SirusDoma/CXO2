#include <Genode/IO/ResourceContext.hpp>
#include <utility>

namespace Gx
{
    const ResourceContext ResourceContext::Default;

    ResourceContext::ResourceContext() :
        m_id(),
        m_cacheMode(CacheMode::None),
        m_resources()
    {
    }

    ResourceContext::ResourceContext(const std::string &id) :
        m_id(id),
        m_cacheMode(CacheMode::None),
        m_resources(nullptr)
    {
    }

    ResourceContext::ResourceContext(const std::string &id, ResourceManager &resources, const CacheMode mode) :
        m_id(id),
        m_cacheMode(mode),
        m_resources(&resources)
    {
    }

    ResourceContext::ResourceContext(const std::string &id, ResourceManager *resources, const CacheMode mode) :
        m_id(id),
        m_cacheMode(mode),
        m_resources(resources)
    {
    }

    ResourceContext ResourceContext::Rebind(const std::string &id, const ResourceContext &ctx)
    {
        if (!ctx.Available())
            return ResourceContext(id);

        return {id, ctx.m_resources, ctx.m_cacheMode};
    }

    const std::string &ResourceContext::GetID() const
    {
        return m_id;
    }

    ResourceManager* ResourceContext::GetResourceManager() const
    {
        return m_resources;
    }

    bool ResourceContext::Available() const
    {
        return m_resources != nullptr;
    }

    CacheMode ResourceContext::GetCacheMode() const
    {
        return m_cacheMode;
    }
}