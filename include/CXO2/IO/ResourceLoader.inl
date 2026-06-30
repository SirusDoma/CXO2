#pragma once

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <fmt/format.h>
#include <string>

namespace Cx
{
    template<typename R>
    Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
    {
        if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
        {
            if (const auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
                return LoadFromMetadata(*metadata, ctx);
        }

        // if (!Gx::FileSystem::Contains(fileName))
        //     return Instantiate(ctx);

        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

        auto& inputStream = *stream.get();
        return LoadFromStream(inputStream, ctx);
    }

    template<typename R>
    Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const
    {
        if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
        {
            if (const auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
                return LoadFromMetadata(*metadata, ctx);
        }

        auto stream = sf::MemoryInputStream(data, size);
        return LoadFromStream(stream, ctx);
    }

    template<typename R>
    Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
        {
            if (const auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
                return LoadFromMetadata(*metadata, ctx);
        }

        const auto size = stream.getSize().value_or(0) - stream.tell().value_or(0);

        auto bytes = std::vector<std::uint8_t>(size);
        if (!stream.read(bytes.data(), size).has_value())
            throw Gx::ResourceLoadException("Failed to load the resource");

        const auto json = Gx::Json::parse(std::string(reinterpret_cast<const char*>(bytes.data()), size));

        return LoadFromJson(json, ctx);
    }

    template<typename R>
    void ResourceLoader<R>::LoadChildren(ObjectContainer& container, const ResourceMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (!metadata.Objects.empty())
        {
            for (auto [key, object] : metadata.Objects)
            {
                auto name = fmt::format("{}/{}", metadata.Name, key);
                auto objectCtx = ResourceContextDecorator::Decorate(Gx::ResourceContext::Rebind(context, name), metadata);

                ObjectLoader::LoadFromJson(name, object, container, objectCtx);
            }
        }
    }
}
