#include <OTwo/IO/Loaders/UI/ToggleButtonLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>

#include <OTwo/Metadata/UI/ToggleButtonMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::ToggleButton> ToggleButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
{
    auto metadata = ToggleButtonMetadata();
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, ctx))
        return nullptr;

    auto states = attributes.at("states");
    std::unordered_map<std::string, Gx::ToggleButton::State> stateMap = {
        { "normal", Gx::ToggleButton::State::Normal },
        { "hover", Gx::ToggleButton::State::Hover },
        { "active", Gx::ToggleButton::State::Active },
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

Gx::ResourcePtr<Gx::ToggleButton> ToggleButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ToggleButtonMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto toggleButton = std::make_unique<Gx::ToggleButton>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        toggleButton->SetTexture(*texture);

    for (auto [state, frame] : metadata->States)
        toggleButton->SetFrame(state, {frame.TexCoords, frame.Color, frame.LocalBounds});

    toggleButton->SetName(metadata->Name);
    toggleButton->SetOrigin(metadata->Origin);
    toggleButton->SetPosition(metadata->Position);
    toggleButton->SetScale(metadata->Scale);
    toggleButton->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(toggleButton.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return toggleButton;
}

