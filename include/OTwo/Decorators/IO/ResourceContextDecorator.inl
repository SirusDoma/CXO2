#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/Graphics/Font.hpp>

#include <SFML/Audio/Music.hpp>

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
R* ResourceContextDecorator::Instantiate(const ResourceMetadata& metadata, const std::string& newID) const
{
    const auto resources = GetResourceManager();
    if (!resources)
        throw Gx::ResourceAccessException(GetID(), "ResourceManager is not set within this context");

    auto require = metadata.Require;
    if (require.empty())
        return nullptr;

    auto instance = Find<R>(metadata);
    if (!instance)
        return nullptr;

    std::string id = newID;
    if (id.empty())
    {
        if constexpr (std::is_base_of_v<R, sf::Texture>)
        {
            if (const auto key = require.find("texture"); key != require.end() && key->second.type() == typeid(Gx::Json))
                id = std::any_cast<Gx::Json>(key->second).get<std::string>();
        }
        else if constexpr (std::is_base_of_v<R, Gx::Font>)
        {
            if (const auto key = require.find("font"); key != require.end() && key->second.type() == typeid(Gx::Json))
                id = std::any_cast<Gx::Json>(key->second).get<std::string>();
        }
        else if constexpr (std::is_base_of_v<R, sf::SoundBuffer>)
        {
            if (const auto key = require.find("sound"); key != require.end() && key->second.type() == typeid(Gx::Json))
                id = std::any_cast<Gx::Json>(key->second).get<std::string>();
        }
        else
        {
            static_assert(TypeChecker<R>::value, "Resource type is not supported");
            throw Gx::NotSupportedException("Resource type is not supported");
        }

        id += "_" + std::to_string(resources->Count<R>());
    }


    return &Store(id, *instance);
}

template<typename R>
R* ResourceContextDecorator::Find() const
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
R* ResourceContextDecorator::Find(const ResourceMetadata& metadata, Gx::CacheMode cacheMode) const
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
    if constexpr (std::is_base_of_v<R, sf::Texture>)
    {
        if (const auto resource = require.find("texture"); resource != require.end())
            id = resolve(resource->second);

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

    if (id.empty())
        return nullptr;

    return &resources->AddFromDeserializer<R>(id, [&] { return Deserialize<R>(id); }, cacheMode);
}
