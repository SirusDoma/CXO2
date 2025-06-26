#include <OTwo/IO/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::Button> ButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
{
    auto metadata = ButtonMetadata();
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    const auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, ctx))
        return nullptr;

    if (!ParseMetadata(attributes, metadata, ctx))
        return nullptr;

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::Button> ButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ButtonMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto button = std::make_unique<Gx::Button>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        button->SetTexture(*texture);

    auto spriteLoader = SpriteLoader();
    for (auto [state, frame] : metadata->States)
        button->SetFrame(state, {frame.TexCoords, frame.LocalBounds});

    button->SetName(metadata->Name);
    button->SetOrigin(metadata->Origin);
    button->SetPosition(metadata->Position);
    button->SetScale(metadata->Scale);
    button->SetRotation(metadata->Rotation);
    button->SetEnabled(metadata->Enabled);
    button->SetVisible(metadata->Visible);

    auto container = ObjectContainer::Decorate(button.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(context, name);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return button;
}

bool ButtonLoader::ParseMetadata(const Gx::Json& attributes, ButtonMetadata& metadata, const Gx::ResourceContext& context)
{
    if (attributes.empty())
        return false;

    SpriteLoader::ParseMetadata(attributes, metadata, context);

    auto states = attributes.at("states");
    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "click", Gx::Button::State::Active },
    };

    if (const auto it = attributes.find("enabled"); it != attributes.end())
        metadata.Enabled = it->get<bool>();

    if (const auto it = attributes.find("visible"); it != attributes.end())
        metadata.Visible = it->get<bool>();

    auto spriteLoader = SpriteLoader();
    for (auto [name, state] : stateMap)
    {
        if (!states.contains(name))
            continue;

        auto stateData = states.at(name);
        ButtonMetadata::ButtonState stateMeta;
        if (!SpriteLoader::ParseMetadata(stateData, stateMeta, context))
            continue;

        if (auto bounds = stateData.find("bounds"); bounds != stateData.end())
        {
            int x      = bounds->find("x") != bounds->end() ? bounds->at("x").get<int>() : 0;
            int y      = bounds->find("y") != bounds->end() ? bounds->at("y").get<int>() : 0;
            int width  = bounds->at("width").get<int>();
            int height = bounds->at("height").get<int>();

            stateMeta.LocalBounds = sf::IntRect{ { x, y }, { width, height } };
        }
        else
        {
            stateMeta.LocalBounds = sf::IntRect{
                { 0, 0 },
                {
                    metadata.TexCoords.size.x,
                    metadata.TexCoords.size.y
                }
            };
        }

        metadata.States[state] = stateMeta;
    }

    return true;
}
