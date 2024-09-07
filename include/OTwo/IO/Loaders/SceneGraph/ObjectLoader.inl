#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

template<typename R>
Gx::ResourcePtr<R> ObjectLoader::LoadResource(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context)
{
    return LoadResource<R, R>(name, json, context);
}

template<typename B, typename R>
Gx::ResourcePtr<B> ObjectLoader::LoadResource(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context)
{
    auto baseLoader = Gx::ResourceLoaderFactory::CreateLoader<R>();
    if (!baseLoader)
        return nullptr;

    // Sounds dangerous, but the type is already guaranteed
    auto loader = reinterpret_cast<ResourceLoader<B>*>(baseLoader.get());
    if (!loader)
        return nullptr;

    Gx::ResourcePtr<B> resource;
    auto ctx = Gx::ResourceContext::Rebind(name, context);
    if (json.type() == Gx::Json::value_t::string)
        resource = loader->LoadFromFile(json.get<std::string>(), ctx);
    else
        resource = loader->LoadFromJson(json, ctx);

    if (!resource)
        return nullptr;

    if constexpr (std::is_base_of_v<Gx::Node, B>)
        resource->SetName(name);

    return resource;
}
