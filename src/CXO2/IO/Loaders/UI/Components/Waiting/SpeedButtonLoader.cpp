#include <CXO2/IO/Loaders/UI/Components/Waiting/SpeedButtonLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/UI/Components/Waiting/SpeedButtonMetadata.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/IO/Loaders/UI/ButtonLoader.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<SpeedButton> SpeedButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = SpeedButtonMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!ButtonLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto speed = attributes.find("speed"); speed != attributes.end() && speed->is_number())
                metadata.Speed = speed->get<float>();

            if (const auto mode = attributes.find("mode"); mode != attributes.end() && mode->is_string())
            {
                if (const auto parsed = magic_enum::enum_cast<SpeedMode>(mode->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                    metadata.Mode = parsed.value();
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<SpeedButton> SpeedButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const SpeedButtonMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto speedButton = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            speedButton->SetTexture(*texture);
            speedButton->SetPosition(metadata->Position.value_or(sf::Vector2f()));

            for (const auto& [state, frame] : metadata->States)
                speedButton->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
        }
        else
        {
            auto bound = sf::IntRect();
            if (metadata->Position.has_value())
            {
                speedButton->SetPosition(*metadata->Position);
            }
            else if (const auto bnd = ctx.Require<sf::IntRect>(*metadata); bnd)
            {
                bound = *bnd;
                speedButton->SetPosition({
                    static_cast<float>(bnd->position.x),
                    static_cast<float>(bnd->position.y),
                });
            }

            if (!metadata->States.empty())
            {
                for (auto [state, frame] : metadata->States)
                {
                    if (!frame.ID.has_value())
                        speedButton->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
                }
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                speedButton->SetTexture(sheet->GetTexture());
                if (sheet->Frames.size() > 1)
                {
                    if (!metadata->Position.has_value() && sheet->Frames[0].position != sf::Vector2i())
                    {
                        auto base = ctx.GetParentBound();
                        speedButton->SetPosition(sf::Vector2f{
                             static_cast<float>(sheet->Frames[0].position.x),
                             static_cast<float>(sheet->Frames[0].position.y)
                        } - sf::Vector2f{
                            static_cast<float>(base.position.x),
                            static_cast<float>(base.position.y)
                        });
                    }
                }

                const auto& frames = sheet->TexCoords;
                auto bounds = std::vector<sf::IntRect>();
                for (std::size_t i = 0; i < frames.size(); i++)
                {
                    if (bound != sf::IntRect())
                        bounds.push_back({ {}, bound.size });
                    else
                        bounds.push_back({ {}, frames[i].size });
                }

                auto states = std::unordered_map<Button::State, Button::Frame>();
                if (!metadata->States.empty())
                {
                    for (auto [state, frame] : metadata->States)
                    {
                        if (frame.ID.has_value())
                        {
                            std::size_t index = frame.ID.value();
                            auto texCoord =  index < sheet->TexCoords.size() ? sheet->TexCoords[index]       : sf::IntRect();
                            auto position =  index < sheet->Frames.size()    ? sheet->Frames[index].position : sf::Vector2i();

                            speedButton->SetFrame(state, {texCoord, { {}, bound.size } });
                            speedButton->SetPosition(sf::Vector2f{
                                static_cast<float>(position.x),
                                static_cast<float>(position.y)
                            });
                        }
                    }
                }
                else if (metadata->States.empty())
                {
                    if (frames.size() > 3)
                    {
                        states = {
                            { Button::State::Normal, { frames[0], bounds[0] } },
                            { Button::State::Hover,  { frames[0], bounds[0] } },
                            { Button::State::Active, { frames[frames.size() - 1], bounds[bounds.size() - 1] } },
                        };
                    }
                    else if (frames.size() == 3)
                    {
                        states = {
                            { Button::State::Normal, { frames[0], bounds[0] } },
                            { Button::State::Hover,  { frames[1], bounds[1] } },
                            { Button::State::Active, { frames[2], bounds[2] } },
                        };
                    }
                    else if (frames.size() == 2)
                    {
                        states = {
                            { Button::State::Hover,  { frames[0], bounds[0] } },
                            { Button::State::Active, { frames[1], bounds[1] } },
                        };
                    }
                    else if (frames.size() == 1)
                    {
                        states = {
                            { Button::State::Active, { frames[0], bounds[0] } },
                        };
                    }
                }

                for (auto [state, frame] : states)
                    speedButton->SetFrame(state, frame);
            }
            else if (metadata->States.empty() && bound != sf::IntRect())
            {
                speedButton->SetFrame( Button::State::Active,
                    { {}, { {}, bound.size } } );
            }
        }

        speedButton->SetSpeed(metadata->Speed);
        speedButton->SetMode(metadata->Mode);
        speedButton->SetName(metadata->Name);
        speedButton->SetOrigin(metadata->Origin);
        speedButton->SetScale(metadata->Scale);
        speedButton->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*speedButton);
        LoadChildren(container, meta, context);

        return speedButton;
    }
}
