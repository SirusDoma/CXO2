#include <Genode/IO/ResourceContext.hpp>
#include <utility>

namespace Gx
{
    const ResourceContext ResourceContext::Default;

    ResourceContext::ResourceContext() :
        m_id(),
        m_resources(),
        m_cacheMode(CacheMode::None)
    {
    }

    ResourceContext::ResourceContext(const std::string &id) :
        m_id(id),
        m_resources(nullptr),
        m_cacheMode(CacheMode::None)
    {
    }

    ResourceContext::ResourceContext(const std::string &id, ResourceManager &resources, CacheMode mode) :
        m_id(std::move(id)),
        m_resources(&resources),
        m_cacheMode(mode)
    {
    }

    ResourceContext::ResourceContext(const std::string &id, ResourceManager *resources, CacheMode mode) :
        m_id(std::move(id)),
        m_resources(resources),
        m_cacheMode(mode)
    {
    }

    ResourceContext ResourceContext::Rebind(const std::string &id, const ResourceContext &ctx)
    {
        if (!ctx.Available())
            return ResourceContext(id);

        return ResourceContext(id, ctx.m_resources, ctx.m_cacheMode);
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