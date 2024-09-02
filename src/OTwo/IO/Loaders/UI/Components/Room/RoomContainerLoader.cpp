#include <OTwo/IO/Loaders/UI/Components/Room/RoomContainerLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/UI/UiContainerMetadata.hpp>

Gx::ResourcePtr<RoomContainer> RoomContainerLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    UiContainerMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (const auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<RoomContainer> RoomContainerLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const UiContainerMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto container = std::make_unique<RoomContainer>();
    auto populator = ObjectContainer::Decorate(container.get());
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

    container->SetOrigin(metadata->Origin);
    container->SetPosition(metadata->Position);
    container->SetScale(metadata->Scale);
    container->SetRotation(metadata->Rotation);

    return container;
}
