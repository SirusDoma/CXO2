#include <OTwo/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/RadioButtonMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::RadioButton> RadioButtonLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    auto metadata = RadioButtonMetadata();
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, ctx))
        return nullptr;

    auto states = attributes.at("states");
    std::unordered_map<std::string, Gx::RadioButton::State> stateMap = {
        { "normal", Gx::RadioButton::State::Normal },
        { "hover",  Gx::RadioButton::State::Hover  },
        { "active", Gx::RadioButton::State::Active },
    };

    auto spriteLoader = SpriteLoader();
    for (auto [name, state] : stateMap)
    {
        if (!states.contains(name))
            continue;

        SpriteMetadata stateMeta;
        if (!SpriteLoader::ParseMetadata(states.at(name), stateMeta, ctx))
            continue;

        metadata.States[state] = stateMeta;
    }

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::RadioButton> RadioButtonLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const RadioButtonMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto radio = std::make_unique<Gx::RadioButton>();
    auto ctx = ResourceContextDecorator::Decorate(context);
    if (auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        radio->SetTexture(*texture);

    auto spriteLoader = SpriteLoader();
    for (auto [state, frame] : metadata->States)
    {
        auto sprite = Gx::Sprite(*spriteLoader.LoadFromMetadata(frame, ctx));
        radio->SetStateFrame(state, {sprite.GetTexCoords(), sprite.GetColor()});
    }

    radio->SetName(metadata->Name);
    radio->SetOrigin(metadata->Origin);
    radio->SetPosition(metadata->Position);
    radio->SetScale(metadata->Scale);
    radio->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(radio.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return radio;
}