#include <OTwo/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/SceneGraph/StateMetadata.hpp>

#include <Genode/UI.hpp>
#include <magic_enum.hpp>

Gx::ResourcePtr<State> StateLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    StateMetadata metadata;
    if (json.find("name") == json.end())
        throw Gx::ResourceLoadException("State must have a name");

    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<State> StateLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const StateMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto state = Create();
    state->SetName(meta.Name);

    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        // Rewire resource manager to the local scene
        auto container = ObjectContainer::Decorate(state.get(), true);
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetResources(), context.GetCacheMode());

        ObjectLoader::Load(name, reference, container, ctx);
    }

    for (auto [key, object] : metadata->Objects)
    {
        // Rewire resource manager to the local scene
        auto container = ObjectContainer::Decorate(state.get());
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetResources(), context.GetCacheMode());

        ObjectLoader::Load(name, object, container, ctx);
    }

    return state;
}
