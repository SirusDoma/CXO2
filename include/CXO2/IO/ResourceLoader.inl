#pragma once

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <fmt/format.h>
#include <string>

namespace Cx
{
    template<typename R>
    template<typename U>
    void ResourceLoader<R>::OnRegistered(const U& id, const Builder&)
    {
        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_same_v<Gx::Node, R>)
            Gx::ResourceLoaderFactory::Map<Gx::Node, R, U>(id);
    }

    template<typename R>
    template<typename U>
    void ResourceLoader<R>::OnRemoved(const U& id)
    {
        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_same_v<Gx::Node, R>)
            Gx::ResourceLoaderFactory::Remove<Gx::Node, U>(id);
    }

    template<typename R>
    Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
        {
            if (const auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
                return LoadFromMetadata(*metadata, ctx);
        }

        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
        {
            if (IsFailSafe())
                return this->Instantiate(ctx);

            throw Gx::ResourceLoadException(fileName.string());
        }

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
        {
            if (IsFailSafe())
                return this->Instantiate(ctx);

            throw Gx::ResourceLoadException(ctx.GetID());
        }

        auto json = Gx::Json();
        try
        {
            json = Gx::Json::parse(std::string(reinterpret_cast<const char*>(bytes.data()), size));
        }
        catch (const Gx::Json::exception&)
        {
            if (IsFailSafe())
                return this->Instantiate(ctx);

            throw;
        }

        return LoadFromJson(json, ctx);
    }

    template<typename R>
    void ResourceLoader<R>::LoadChildren(SceneComposer& composer, const ResourceMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (!metadata.Objects.empty())
        {
            for (auto [key, object] : metadata.Objects)
            {
                auto name = fmt::format("{}/{}", metadata.Name, key);
                auto objectCtx = ResourceContextDecorator::Decorate(Gx::ResourceContext::Rebind(context, name), metadata);

                composer.Add(name, object, objectCtx);
            }
        }
    }
}
