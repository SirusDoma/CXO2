#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/IO/TextureCompiler.hpp>
#include <OTwo/Metadata/Legacy/ControlList.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/Graphics/Font.hpp>
#include <Genode/UI/Control.hpp>

#include <SFML/Audio/SoundBuffer.hpp>

#include <fmt/format.h>

#include <any>

template<typename T>
struct TypeChecker : std::false_type
{ };

template<typename R>
Gx::ResourcePtr<R> ResourceContextDecorator::Deserialize(const std::string& id) const
{
    auto stream = Gx::FileSystem::Open(id);
    if (!stream)
        throw Gx::ResourceLoadException(id, "Failed to load resource");

    const auto loader = Gx::ResourceLoaderFactory::CreateLoader<R>();
    if (!loader)
        throw Gx::ResourceLoadException(id, "There's no [ResourceLoader] for [" + std::string(typeid(R).name()) + "] type");

    auto resource = loader->LoadFromStream(*stream, *this);
    if (!loader->IsStreaming())
        return resource;

    auto source  = stream.release();
    auto deleter = resource.get_deleter();
    resource = Gx::ResourcePtr<R>(resource.release(), [source, deleter] (auto ptr) {
       delete source;
       deleter(ptr);
    });

    return resource;
}

template<typename R>
R* ResourceContextDecorator::Require() const
{
    const auto resources = GetResourceManager();
    if (!resources)
        throw Gx::ResourceAccessException(GetID(), "ResourceManager is not set within this context.");

    auto metadata = resources->AddFromDeserializer<ResourceMetadata>(GetID(), [&] { return Deserialize<ResourceMetadata>(GetID()); });
    if constexpr (std::is_base_of_v<R, ResourceMetadata>)
        return *metadata;

    return Find<R>(metadata);
}

template<typename R>
R* ResourceContextDecorator::Require(const ResourceMetadata& metadata, Gx::CacheMode cacheMode) const
{
    const auto resources = GetResourceManager();
    if (!resources)
        throw Gx::ResourceAccessException(GetID(), "ResourceManager is not set within this context.");

    auto require = metadata.Require;
    if (require.empty())
        return nullptr;

    auto resolve = [] (const std::any& resource) -> std::string
    {
        if (resource.type() == typeid(Gx::Json))
            return std::any_cast<Gx::Json>(resource).get<std::string>();

        if (resource.type() == typeid(std::string))
            return std::any_cast<std::string>(resource);

        return std::string();
    };

    std::string id;
    bool global = true;
    if constexpr (std::is_base_of_v<R, sf::Texture>)
    {
        if (const auto resource = require.find("texture"); resource != require.end())
            id = resolve(resource->second);

    }
    else if constexpr (std::is_base_of_v<R, SpriteSheet>)
    {
        if (const auto resource = require.find("sheet"); resource != require.end())
        {
            id = resolve(resource->second);
        }
        else if (const auto ref = require.find("refID"); ref != require.end())
        {
            auto refID = resolve(ref->second);
            if (auto control = resources->Find<ControlList::Control>(refID))
                id = control->GetParam();
        }
    }
    else if constexpr (std::is_base_of_v<R, sf::IntRect>)
    {
        if (const auto bound = require.find("bndID"); bound != require.end())
            id = resolve(bound->second);
        else if (const auto globalBound = require.find("globalBndID"); globalBound != require.end())
            id = resolve(globalBound->second);
        else if (const auto localBound = require.find("localBndID"); localBound != require.end())
        {
            id = resolve(localBound->second);
            global = false;
        }
    }
    else if constexpr (std::is_base_of_v<R, Gx::Font>)
    {
        if (const auto resource = require.find("font"); resource != require.end())
            id = resolve(resource->second);
    }
    else if constexpr (std::is_base_of_v<R, sf::SoundBuffer>)
    {
        if (const auto resource = require.find("sound"); resource != require.end())
            id = resolve(resource->second);
    }
    else
    {
        static_assert(TypeChecker<R>::value, "Resource type is not supported.");
        throw Gx::NotSupportedException("Resource type is not supported.");
    }

    if (Gx::StringHelper::StartsWith(id, "_"))
        id = Format(id.substr(1), metadata.SourceFormat);

    if (id.empty())
        return nullptr;

    // TODO: try-catch return nullptr
    auto resource = &resources->AddFromDeserializer<R>(id, [&] { return Deserialize<R>(id); }, cacheMode);

    // Genode Scene Graph system only work with local bound
    // Convert global bound to local bound
    if constexpr (std::is_base_of_v<R, sf::IntRect>)
    {
        const auto parent = GetParent();
        if (global && parent)
        {
            if (auto cache = resources->Find<R>(fmt::format("{}-normalized", id)))
                return cache;

            auto parentID = std::string();
            require = parent->Require;

            if (const auto bound = require.find("bndID"); bound != require.end())
                parentID = resolve(bound->second);
            else if (const auto globalBound = require.find("globalBndID"); globalBound != require.end())
                parentID = resolve(globalBound->second);

            if (!parentID.empty())
            {
                if (auto bound = resources->Find<R>(parentID))
                {
                    resource = &resources->AddFromDeserializer<R>(fmt::format("{}-normalized", id), [&]
                    {
                        return std::make_unique<sf::IntRect>(
                            resource->position - bound->position,
                            resource->size
                        );
                    }, cacheMode);
                }
            }
        }
    }

    return resource;
}
