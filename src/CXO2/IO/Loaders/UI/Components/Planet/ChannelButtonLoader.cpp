#include <CXO2/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Metadata/UI/Components/Planet/ChannelButtonMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<ChannelButton> ChannelButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = ChannelButtonMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto modeAttributes = attributes.find("mode"); modeAttributes != attributes.end())
            {
                for (auto [key, data]: modeAttributes->items())
                {
                    ChannelButton::Mode mode;
                    if (auto parsed = magic_enum::enum_cast<ChannelButton::Mode>(key,magic_enum::case_insensitive); parsed.has_value())
                        mode = parsed.value();
                    else
                        continue;

                    metadata.States[mode] = {};

                    if (const auto itStates = data.find("states"); itStates != data.end())
                    {
                        const auto& states = itStates.value();
                        std::unordered_map<std::string, Gx::RadioButton::State> stateMap =
                        {
                            {"normal", Gx::RadioButton::State::Normal},
                            {"hover",  Gx::RadioButton::State::Hover},
                            {"active", Gx::RadioButton::State::Active},
                        };

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
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<ChannelButton> ChannelButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ChannelButtonMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto channelButton = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata))
        {
            channelButton->SetTexture(*texture);
            channelButton->SetPosition(metadata->Position.value_or(sf::Vector2f()));

            for (const auto& [mode, data] : metadata->States)
            {
                for (auto [state, frame] : data)
                    channelButton->AddStateFrame(mode, state, frame);
            }
        }
        else
        {
            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                channelButton->SetTexture(sheet->GetTexture());
                for (const auto& [mode, data] : metadata->States)
                {
                    if (data.empty())
                    {
                        if (sheet->TexCoords.size() > 1)
                            channelButton->AddStateFrame(mode, Gx::Button::State::Normal, sheet->TexCoords[0]);

                        if (sheet->TexCoords.size() > 2)
                            channelButton->AddStateFrame(mode, Gx::Button::State::Hover, sheet->TexCoords[1]);

                        const std::size_t index = 2 + static_cast<int>(mode);
                        if (index < sheet->TexCoords.size())
                            channelButton->AddStateFrame(mode, Gx::Button::State::Active, sheet->TexCoords[index]);
                    }
                    else
                    {
                        for (auto [state, frame] : data)
                            channelButton->AddStateFrame(mode, state, frame);
                    }
                }
            }

            if (metadata->Position.has_value())
            {
                channelButton->SetPosition(*metadata->Position);
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                channelButton->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }
        }


        channelButton->SetName(metadata->Name);
        channelButton->SetOrigin(metadata->Origin);
        channelButton->SetScale(metadata->Scale);
        channelButton->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*channelButton);
        LoadChildren(container, meta, context);

        return channelButton;
    }
}
