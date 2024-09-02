#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

template<typename R>
Gx::ResourcePtr<R> ObjectLoader::LoadResource(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context)
{
    auto baseLoader = Gx::ResourceLoaderFactory::GetLoader<R>();
    if (!baseLoader)
        return nullptr;

    auto loader = dynamic_cast<ResourceLoader<R>*>(baseLoader.get());
    if (!loader)
        return nullptr;

    Gx::ResourcePtr<R> resource;
    auto ctx = Gx::ResourceContext::Rebind(name, context);
    if (json.type() == Gx::Json::value_t::string)
        resource = loader->LoadFromFile(json.get<std::string>(), ctx);
    else
        resource = loader->LoadFromJson(json, ctx);

    if (!resource)
        return nullptr;

    if constexpr (std::is_base_of_v<Gx::Node, R>)
        resource->SetName(name);

    return resource;
}
