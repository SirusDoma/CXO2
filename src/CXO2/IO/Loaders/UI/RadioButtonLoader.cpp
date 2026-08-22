#include <CXO2/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/UI/RadioButtonMetadata.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/IO/Loaders/UI/ButtonLoader.hpp>

namespace Cx
{
    Gx::ResourcePtr<RadioButton> RadioButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = RadioButtonMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!ButtonLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<RadioButton> RadioButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const RadioButtonMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto radio = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            radio->SetTexture(*texture);
            radio->SetPosition(metadata->Position.value_or(sf::Vector2f()));

            for (const auto& [state, frame] : metadata->States)
                radio->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
        }
        else
        {
            auto bound = sf::IntRect();
            if (metadata->Position.has_value())
            {
                radio->SetPosition(*metadata->Position);
            }
            else if (const auto bnd = ctx.Require<sf::IntRect>(*metadata); bnd)
            {
                bound = *bnd;
                radio->SetPosition({
                    static_cast<float>(bnd->position.x),
                    static_cast<float>(bnd->position.y),
                });
            }

            if (!metadata->States.empty())
            {
                for (auto [state, frame] : metadata->States)
                {
                    if (!frame.ID.has_value())
                        radio->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
                }
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                radio->SetTexture(sheet->GetTexture());
                if (sheet->Frames.size() > 1)
                {
                    if (!metadata->Position.has_value() && sheet->Frames[0].position != sf::Vector2i())
                    {
                        auto base = ctx.GetParentBound();
                        radio->SetPosition(sf::Vector2f{
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

                            radio->SetFrame(state, {texCoord, { {}, bound.size } });
                            radio->SetPosition(sf::Vector2f{
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
                    radio->SetFrame(state, frame);
            }
            else if (metadata->States.empty() && bound != sf::IntRect())
            {
                radio->SetFrame( Button::State::Active,
                    { {}, { {}, bound.size } } );
            }
        }

        radio->SetName(metadata->Name);
        radio->SetOrigin(metadata->Origin);
        radio->SetScale(metadata->Scale);
        radio->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*radio);
        LoadChildren(container, meta, context);

        return radio;
    }
}
