#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <OTwo/Metadata/UI/UiContainerMetadata.hpp>

Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    UiContainerMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const UiContainerMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto container = std::make_unique<Gx::UiContainer>();
    auto populator = ObjectPopulator::Decorate(container.get());
    auto ctx       = ResourceContextDecorator::Decorate(context);
    container->SetName(metadata->Name);

    auto metaLoader = MetadataLoader();
    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, reference, populator, ctx);
    }

    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, object, populator, ctx);
    }

    return container;
}

