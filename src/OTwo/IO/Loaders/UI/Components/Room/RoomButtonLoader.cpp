#include <OTwo/IO/Loaders/UI/Components/Room/RoomButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<RoomButton> RoomButtonLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    ImageMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    const auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (!ImageLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<RoomButton> RoomButtonLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ImageMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto button = std::make_unique<RoomButton>();
    const auto ctx = ResourceContextDecorator::Decorate(context);

    if (metadata->Frames.size() > 0)
    {
        for (auto frame : metadata->Frames)
            button->AddFrame(frame.first, frame.second);
    }
    else
        button->SetTexCoords(metadata->TexCoords);

    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        button->SetTexture(*texture);

    button->SetName(metadata->Name);
    button->SetColor(metadata->Color);
    button->SetOrigin(metadata->Origin);
    button->SetPosition(metadata->Position);
    button->SetScale(metadata->Scale);
    button->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(button.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return button;
}
