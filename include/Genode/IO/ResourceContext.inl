#pragma once

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/IOException.hpp>

namespace Gx
{
    template<typename Ctx>
    inline std::enable_if_t<std::is_base_of_v<ResourceContext, Ctx>, Ctx>
    ResourceContext::Rebind(const Ctx& ctx, const std::string& id)
    {
        auto pctx       = Ctx(ctx);
        const auto rctx = static_cast<ResourceContext*>(&pctx);
        rctx->m_id      = id;

        return pctx;
    }

    template<typename Ctx>
    inline std::enable_if_t<std::is_base_of_v<ResourceContext, Ctx>, Ctx>
    ResourceContext::Rebind(const Ctx& ctx, ResourceManager& resources)
    {
        auto pctx       = Ctx(const_cast<Ctx&>(ctx));
        const auto rctx = static_cast<ResourceContext*>(&pctx);
        rctx->m_resources = &resources;

        return pctx;
    }

    template<typename R>
    R* ResourceContext::Find(const std::string& id) const
    {
        if (!m_resources)
            throw ResourceAccessException(id, "ResourceManager is not set within this context");

        return m_resources->Find<R>(id);
    }

    template<typename R>
    R& ResourceContext::Acquire(const std::string& id) const
    {
        if (!m_resources)
            throw ResourceAccessException(id, "ResourceManager is not set within this context");

        auto resource = m_resources->Find<R>(id);
        if (resource)
            return *resource;

        return m_resources->AddFromFile<R>(id, id, CacheMode::Reuse);
    }

    template<typename R>
    R& ResourceContext::Acquire(const std::string& id, const std::string& path) const
    {
        if (!m_resources)
            throw ResourceAccessException(id, "ResourceManager is not available within this context.");

        return m_resources->AddFromFile<R>(id, path, CacheMode::Reuse);
    }

    template<typename R>
    R& ResourceContext::Acquire(const std::string& id, const void* data, std::size_t dataSize) const
    {
        if (!m_resources)
            throw ResourceAccessException(id, "ResourceManager is not available within this context.");

        return m_resources->AddFromMemory<R>(id, data, dataSize, CacheMode::Reuse);
    }

    template<typename R>
    R& ResourceContext::Acquire(const std::string& id, sf::InputStream& stream) const
    {
        if (!m_resources)
            throw ResourceAccessException(id, "ResourceManager is not available within this context.");

        return m_resources->AddFromStream<R>(id, stream, CacheMode::Reuse);
    }

    template<typename R>
    R& ResourceContext::Store(const std::string& id, R& resource) const
    {
        if (!m_resources)
            throw ResourceStoreException(id, "ResourceManager is not available within this context.");

        return m_resources->Store(id, resource, CacheMode::Update);
    }

    template<typename R>
    R& ResourceContext::Store(const std::string& id, ResourcePtr<R> resource) const
    {
        if (!m_resources)
            throw ResourceStoreException(id, "ResourceManager is not available within this context.");

        return m_resources->Store(id, std::move(resource), CacheMode::Update);
    }
}