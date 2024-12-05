#include <OTwo/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/RadioButtonMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::RadioButton> RadioButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
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

        auto stateData = states.at(name);
        ButtonMetadata::ButtonState stateMeta;
        if (!SpriteLoader::ParseMetadata(stateData, stateMeta, ctx))
            continue;

        if (auto b = stateData.find("bounds"); b != stateData.end())
        {
            float width  = b->at("width").get<float>();
            float height = b->at("height").get<float>();

            stateMeta.LocalBounds = { width, height };
        }
        else
        {
            stateMeta.LocalBounds = {
                static_cast<float>(stateMeta.TexCoords.size.x),
                static_cast<float>(stateMeta.TexCoords.size.y)
            };
        }

        metadata.States[state] = stateMeta;
    }

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::RadioButton> RadioButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const RadioButtonMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto radio = std::make_unique<Gx::RadioButton>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        radio->SetTexture(*texture);

    for (auto [state, frame] : metadata->States)
        radio->SetFrame(state, {frame.TexCoords, frame.Color, frame.LocalBounds});

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
            auto objectCtx = Gx::ResourceContext::Rebind(context, name);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return radio;
}