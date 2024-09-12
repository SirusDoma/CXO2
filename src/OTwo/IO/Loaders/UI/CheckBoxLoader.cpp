#include <OTwo/IO/Loaders/UI/CheckBoxLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>

#include <OTwo/Metadata/UI/CheckBoxMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::CheckBox> CheckBoxLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
{
    auto metadata = CheckBoxMetadata();
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, ctx))
        return nullptr;

    auto states = attributes.at("states");
    std::unordered_map<std::string, Gx::CheckBox::State> stateMap = {
        { "normal", Gx::CheckBox::State::Normal },
        { "hover", Gx::CheckBox::State::Hover },
        { "active", Gx::CheckBox::State::Active },
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

Gx::ResourcePtr<Gx::CheckBox> CheckBoxLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    auto metadata = dynamic_cast<const CheckBoxMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto checkBox = std::make_unique<Gx::CheckBox>();
    auto ctx = ResourceContextDecorator::Decorate(context);
    if (auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        checkBox->SetTexture(*texture);

    auto spriteLoader = SpriteLoader();
    for (auto [state, frame] : metadata->States)
    {
        auto sprite = Gx::Sprite(*spriteLoader.LoadFromMetadata(frame, ctx));
        checkBox->SetStateFrame(state, {sprite.GetTexCoords(), sprite.GetColor()});
    }

    checkBox->SetName(metadata->Name);
    checkBox->SetOrigin(metadata->Origin);
    checkBox->SetPosition(metadata->Position);
    checkBox->SetScale(metadata->Scale);
    checkBox->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(checkBox.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return checkBox;
}

