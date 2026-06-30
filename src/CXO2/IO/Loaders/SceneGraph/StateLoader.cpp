#include <CXO2/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <CXO2/IO/TextureCacheBuilder.hpp>
#include <CXO2/Metadata/SceneGraph/StateMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    StateLoader::StateLoader(Gx::ResourceManager& resources)
        : m_resources(resources)
    {
    }

    Gx::ResourcePtr<State> StateLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
    {
        StateMetadata metadata;
        if (json.find("name") == json.end())
            throw Gx::ResourceLoadException("State must have a name");

        if (!MetadataLoader::Parse(json, metadata, ctx))
            return nullptr;

        return LoadFromMetadata(metadata, ctx);
    }

    Gx::ResourcePtr<State> StateLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        auto metadata = dynamic_cast<const StateMetadata*>(&meta);
        if (metadata == nullptr)
            return nullptr;

        auto state = Instantiate(context);
        state->SetName(meta.Name);

        for (auto [key, value] : meta.Require)
        {
            auto reference = std::any_cast<Gx::Json>(value);
            if (reference.type() != Gx::Json::value_t::string)
                continue;

            // Rewire resource manager to the local scene
            auto container = ObjectContainer::Decorate(state.get(), true);
            auto name = fmt::format("{}/{}", meta.Name, key);
            auto ctx  = Gx::ResourceContext(name, m_resources, context.GetCacheMode());
            for (const auto& [key, value] : context.GetProperties())
                ctx.SetProperty(key, value);

            ObjectLoader::LoadFromJson(name, reference, container, ctx);
        }

        for (const auto& [key, object] : metadata->Objects)
        {
            // Rewire resource manager to the local scene
            auto container = ObjectContainer::Decorate(state.get());
            auto name = fmt::format("{}/{}", meta.Name, key);
            auto ctx  = Gx::ResourceContext(name, m_resources, context.GetCacheMode());
            for (const auto& [key, value] : context.GetProperties())
                ctx.SetProperty(key, value);

            ObjectLoader::LoadFromJson(name, object, container, ctx);
        }

        return state;
    }
}
