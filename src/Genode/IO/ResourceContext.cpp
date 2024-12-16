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

    ResourceContext::ResourceContext(std::string  id) :
        m_id(std::move(id)),
        m_cacheMode(CacheMode::None),
        m_resources(nullptr)
    {
    }

    ResourceContext::ResourceContext(std::string  id, ResourceManager& resources, const CacheMode mode) :
        m_id(std::move(id)),
        m_cacheMode(mode),
        m_resources(&resources)
    {
    }

    ResourceContext::ResourceContext(std::string  id, ResourceManager* resources, const CacheMode mode) :
        m_id(std::move(id)),
        m_cacheMode(mode),
        m_resources(resources)
    {
    }

    const std::string& ResourceContext::GetID() const
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

    void ResourceContext::Bind(ResourceManager& resources)
    {
        if (m_resources)
            throw InvalidOperationException("Context is already bound to a resource manager");

        m_resources = &resources;
    }

    void ResourceContext::Unbind()
    {
        m_resources = nullptr;
    }
}
