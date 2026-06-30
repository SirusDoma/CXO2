#include <CXO2/IO/Loaders/UI/ButtonLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::Button> ButtonLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = ButtonMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (!ParseMetadata(attributes, metadata, context))
                return Instantiate(context);
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::Button> ButtonLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ButtonMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException("The specified metadata is incompatible");

        auto button = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            button->SetTexture(*texture);
            button->SetPosition(metadata->Position);

            for (const auto& [state, frame] : metadata->States)
                button->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
        }
        else
        {
            auto bound = sf::IntRect();
            if (metadata->Position != sf::Vector2f())
            {
                button->SetPosition(metadata->Position);
            }
            else if (const auto bnd = ctx.Require<sf::IntRect>(*metadata); bnd)
            {
                bound = *bnd;
                button->SetPosition(sf::Vector2f{
                    static_cast<float>(bnd->position.x),
                    static_cast<float>(bnd->position.y),
                });
            }

            if (!metadata->States.empty())
            {
                for (auto [state, frame] : metadata->States)
                {
                    if (!frame.ID.has_value())
                        button->SetFrame(state, {frame.TexCoords, frame.LocalBounds});
                }
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                button->SetTexture(sheet->GetTexture());
                if (sheet->Frames.size() > 1)
                {
                    if (metadata->Position == sf::Vector2f() && sheet->Frames[0].position != sf::Vector2i())
                    {
                        auto base = ctx.GetParentBound();
                        button->SetPosition(sf::Vector2f{
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

                auto states = std::unordered_map<Gx::Button::State, Gx::Button::Frame>();
                if (!metadata->States.empty())
                {
                    for (auto [state, frame] : metadata->States)
                    {
                        if (frame.ID.has_value())
                        {
                            std::size_t index = frame.ID.value();
                            auto texCoord =  index < sheet->TexCoords.size() ? sheet->TexCoords[index]       : sf::IntRect();
                            auto position =  index < sheet->Frames.size()    ? sheet->Frames[index].position : sf::Vector2i();

                            button->SetFrame(state, {texCoord, { {}, bound.size } });
                            button->SetPosition(sf::Vector2f{
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
                            { Gx::Button::State::Normal, { frames[0], bounds[0] } },
                            { Gx::Button::State::Hover,  { frames[0], bounds[0] } },
                            { Gx::Button::State::Active, { frames[frames.size() - 1], bounds[bounds.size() - 1] } },
                        };
                    }
                    else if (frames.size() == 3)
                    {
                        states = {
                            { Gx::Button::State::Normal, { frames[0], bounds[0] } },
                            { Gx::Button::State::Hover,  { frames[1], bounds[1] } },
                            { Gx::Button::State::Active, { frames[2], bounds[2] } },
                        };
                    }
                    else if (frames.size() == 2)
                    {
                        states = {
                            { Gx::Button::State::Hover,  { frames[0], bounds[0] } },
                            { Gx::Button::State::Active, { frames[1], bounds[1] } },
                        };
                    }
                    else if (frames.size() == 1)
                    {
                        states = {
                            { Gx::Button::State::Active, { frames[0], bounds[0] } },
                        };
                    }
                }

                for (auto [state, frame] : states)
                    button->SetFrame(state, frame);
            }
            else if (metadata->States.empty() && bound != sf::IntRect())
            {
                button->SetFrame( Gx::Button::State::Active,
                    { {}, { {}, bound.size } } );
            }
        }

        button->SetName(metadata->Name);
        button->SetOrigin(metadata->Origin);
        button->SetScale(metadata->Scale);
        button->SetRotation(metadata->Rotation);

        auto container = ObjectContainer::Decorate(button.get());
        LoadChildren(container, meta, context);

        return button;
    }

    bool ButtonLoader::ParseMetadata(const Gx::Json& attributes, ButtonMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (attributes.empty())
            return false;

        SpriteLoader::ParseMetadata(attributes, metadata, context);
        if (const auto it = attributes.find("enabled"); it != attributes.end())
            metadata.Enabled = it->get<bool>();

        if (const auto it = attributes.find("visible"); it != attributes.end())
            metadata.Visible = it->get<bool>();

        if (const auto it = attributes.find("states"); it != attributes.end())
        {
            const auto& states = attributes.at("states");
            std::unordered_map<std::string, Gx::Button::State> stateMap = {
                { "normal", Gx::Button::State::Normal },
                { "hover", Gx::Button::State::Hover },
                { metadata.Type == ResourceMetadata::ResourceType::Button ? "click" : "active", Gx::Button::State::Active },
            };

            for (auto [name, state] : stateMap)
            {
                if (!states.contains(name))
                    continue;

                auto statePayload = states.at(name);
                ButtonMetadata::ButtonState stateData;
                if (!SpriteLoader::ParseMetadata(statePayload, stateData, context))
                    continue;

                if (const auto bounds = statePayload.find("bounds"); bounds != statePayload.end())
                {
                    int x      = bounds->find("x") != bounds->end() ? bounds->at("x").get<int>() : 0;
                    int y      = bounds->find("y") != bounds->end() ? bounds->at("y").get<int>() : 0;
                    int width  = bounds->at("width").get<int>();
                    int height = bounds->at("height").get<int>();

                    stateData.LocalBounds = sf::IntRect{ { x, y }, { width, height } };
                }
                else if (const auto id = statePayload.find("id"); id != statePayload.end())
                {
                    stateData.ID = id->get<std::uint16_t>();
                }
                else if (stateData.TexCoords != sf::IntRect())
                {
                    stateData.LocalBounds = sf::IntRect{
                        { 0, 0 },
                        {
                            stateData.TexCoords.size.x,
                            stateData.TexCoords.size.y
                        }
                    };
                }
                else
                {
                    stateData.LocalBounds = sf::IntRect{
                        { 0, 0 },
                        {
                            metadata.TexCoords.size.x,
                            metadata.TexCoords.size.y
                        }
                    };
                }

                metadata.States[state] = stateData;
            }
        }

        return true;
    }
}
