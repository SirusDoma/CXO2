#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/UI/Components/Planet/ChannelButtonMetadata.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<ChannelButton> ChannelButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    auto metadata = ChannelButtonMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto modeAttributes = attributes.find("mode"); modeAttributes != attributes.end())
    {
        for (auto [key, data]: modeAttributes->items())
        {
            ChannelButton::Mode mode;
            if (auto parsed = magic_enum::enum_cast<ChannelButton::Mode>(key,magic_enum::case_insensitive); parsed.has_value())
                mode = parsed.value();
            else
                continue;

            auto states = data.at("states");
            std::unordered_map<std::string, Gx::RadioButton::State> stateMap =
            {
                {"normal", Gx::RadioButton::State::Normal},
                {"hover",  Gx::RadioButton::State::Hover},
                {"active", Gx::RadioButton::State::Active},
            };

            auto spriteLoader = SpriteLoader();
            for (auto [name, state]: stateMap)
            {
                if (!states.contains(name))
                    continue;

                SpriteMetadata stateMeta;
                if (!SpriteLoader::ParseMetadata(states.at(name), stateMeta, context))
                    continue;

                metadata.States[mode][state] = stateMeta.TexCoords;
            }
        }
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<ChannelButton> ChannelButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ChannelButtonMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto channelButton = std::make_unique<ChannelButton>();
    const auto acquirer = ResourceContextDecorator::Decorate(context);
    if (const auto texture = acquirer.Find<sf::Texture>(*metadata); texture)
        channelButton->SetTexture(*texture);

    auto spriteLoader = SpriteLoader();
    for (auto [mode, data] : metadata->States)
    {
        for (auto [state, frame] : data)
            channelButton->AddStateFrame(mode, state, frame);
    }

    channelButton->SetName(metadata->Name);
    channelButton->SetOrigin(metadata->Origin);
    channelButton->SetPosition(metadata->Position);
    channelButton->SetScale(metadata->Scale);
    channelButton->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(channelButton.get());
    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext::Rebind(name, context);

        ObjectLoader::Load(name, object, container, ctx);
    }

    return channelButton;
}
