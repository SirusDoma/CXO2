#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/Metadata/Graphics/AnimationMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

Gx::ResourcePtr<Gx::Animation> AnimationLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    AnimationMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return Instantiate(context);

    if (const auto attr = json.find("attributes"); attr != json.end())
    {
        auto& attributes = attr.value();
        if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
            return Instantiate(context);

        if (const auto alignment = attributes.find("alignment"); alignment != attributes.end())
        {
            if (auto parsed = magic_enum::enum_cast<AnimationMetadata::Alignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.Align = parsed.value();
        }

        if (const auto inherit = attributes.find("inherit"); inherit != attributes.end())
            metadata.Inherit = inherit->get<bool>();

        if (const auto frames = attributes.find("frames"); frames != attributes.end())
        {
            for (const auto& frame : frames->items())
            {
                SpriteMetadata frameMetadata;
                if (!SpriteLoader::ParseMetadata(frame.value(), frameMetadata, context))
                    continue;

                auto transform = frame.value().find("transform");

                std::optional position = frameMetadata.Position;
                if (transform == frame.value().end() || transform.value().find("position") == transform->end())
                    position = std::nullopt;

                std::optional scale = frameMetadata.Scale;
                if (transform == frame.value().end() || transform.value().find("scale") == transform->end())
                    scale = std::nullopt;

                std::optional rotation = frameMetadata.Rotation;
                if (transform == frame.value().end() || transform.value().find("rotation") == transform->end())
                    rotation = std::nullopt;

                std::optional origin = frameMetadata.Origin;
                if (transform == frame.value().end() || transform.value().find("origin") == transform->end())
                {
                    if (metadata.Align != AnimationMetadata::Alignment::None && frameMetadata.TexCoords != sf::IntRect())
                    {
                        origin = sf::Vector2f{
                            static_cast<float>(frameMetadata.TexCoords.size.x) / 2.f,
                            static_cast<float>(frameMetadata.TexCoords.size.y) / 2.f
                        };

                        if (metadata.Align == AnimationMetadata::Alignment::HorizontalCenter)
                            origin->y = 0;

                        if (metadata.Align == AnimationMetadata::Alignment::VerticalCenter)
                            origin->x = 0;
                    }
                    else
                        origin = std::nullopt;
                }

                std::optional<std::uint32_t> id = std::nullopt;
                if (auto frameID = frame.value().find("id"); frameID != frame.value().end())
                    id = frameID->get<std::uint32_t>();

                metadata.Frames.push_back(AnimationMetadata::Frame
                {
                    frameMetadata.TexCoords,
                    origin,
                    position,
                    rotation,
                    scale,
                    id
                });
            }
        }

        const auto loop = attributes.find("isLoop");
        metadata.IsLoop = loop != attributes.end() && loop->get<bool>();

        const auto repeatCount = attributes.find("repeatCount");
        metadata.RepeatCount = repeatCount != attributes.end() ? repeatCount->get<unsigned int>() : 0;

        const auto speed = attributes.find("speed");
        metadata.Speed = speed != attributes.end() ? speed->get<float>() : 1.0f;

        if (const auto duration = attributes.find("duration"); duration != attributes.end())
            metadata.Duration = sf::milliseconds(duration->get<unsigned int>());
        else if (metadata.Frames.size() > 1)
            metadata.Duration = sf::milliseconds(metadata.Frames.size() * 60);
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Animation> AnimationLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const AnimationMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto animation = Instantiate(context);
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
    {
        animation->SetTexture(*texture);

        for (const auto& frame : metadata->Frames)
            animation->AddFrame(frame);

        if (metadata->TexCoords != sf::IntRect())
            animation->SetTexCoords(metadata->TexCoords);

        animation->SetDuration(metadata->Duration);
        animation->SetPosition(metadata->Position);
    }
    else
    {
        auto position = sf::Vector2f();
        if (metadata->Position != sf::Vector2f())
        {
            position = metadata->Position;
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            position ={
                static_cast<float>(bound->position.x),
                static_cast<float>(bound->position.y),
            };
        }

        animation->SetPosition(position);
        if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
        {
            animation->SetTexture(sheet->GetTexture());
            if (metadata->TexCoords != sf::IntRect())
                animation->SetTexCoords(metadata->TexCoords);
            else
                animation->SetTexCoords(sheet->TexCoords[0]);

            if (!metadata->Frames.empty())
            {
                for (std::size_t i = 0; i < metadata->Frames.size(); i++)
                {
                    auto frame = metadata->Frames[i];
                    if (!frame.Position.has_value())
                    {
                        if (metadata->Position != sf::Vector2f())
                        {
                            frame.Position = metadata->Position;
                        }
                        else if (i < sheet->Frames.size())
                        {
                            const auto base = ctx.GetParentBound();
                            frame.Position = {
                                static_cast<float>(sheet->Frames[i].position.x - base.position.x),
                                static_cast<float>(sheet->Frames[i].position.y - base.position.y),
                            };
                        }
                    }

                    if (frame.ID.has_value())
                        frame.TexCoords = sheet->TexCoords[frame.ID.value()];

                    if (metadata->Align != AnimationMetadata::Alignment::None && frame.Origin == sf::Vector2f())
                    {
                        auto origin = sf::Vector2f{
                            static_cast<float>(frame.TexCoords.size.x) / 2.f,
                            static_cast<float>(frame.TexCoords.size.y) / 2.f
                        };

                        if (metadata->Align == AnimationMetadata::Alignment::HorizontalCenter)
                            origin.y = 0;

                        if (metadata->Align == AnimationMetadata::Alignment::VerticalCenter)
                            origin.x = 0;

                        frame.Origin = origin;
                    }

                    animation->AddFrame(frame);
                }
            }
            else if (metadata->Inherit)
            {
                for (std::size_t i = 0; i < sheet->Frames.size(); i++)
                {
                    const auto& frame = sheet->Frames[i];
                    const auto& texCoords = sheet->TexCoords[i];

                    position = sf::Vector2f{
                        static_cast<float>(frame.position.x),
                        static_cast<float>(frame.position.y),
                    };

                    animation->AddFrame(Gx::Animation::Frame
                    {
                        texCoords,
                        std::nullopt,
                        position,
                        std::nullopt,
                        std::nullopt
                    });
                }
            }
            else
            {
                bool isFrameEmpty = true;
                for (std::size_t i = 0; i < sheet->Frames.size(); i++)
                {
                    isFrameEmpty = sheet->Frames[i].position == sf::Vector2i();
                    if (!isFrameEmpty)
                        break;
                }

                for (std::size_t i = 0; i < sheet->TexCoords.size(); i++)
                {
                    const auto& frame = sheet->TexCoords[i];
                    const auto base = ctx.GetParentBound();

                    std::optional<sf::Vector2f> origin = std::nullopt;
                    if (i < sheet->Frames.size())
                    {
                        if (!isFrameEmpty && metadata->Position == sf::Vector2f())
                        {
                            position = {
                                static_cast<float>(sheet->Frames[i].position.x - base.position.x),
                                static_cast<float>(sheet->Frames[i].position.y - base.position.y),
                            };
                        }

                        if (metadata->Align != AnimationMetadata::Alignment::None && metadata->Origin == sf::Vector2f())
                        {
                            origin = sf::Vector2f{
                                static_cast<float>(sheet->Frames[i].size.x) / 2.f,
                                static_cast<float>(sheet->Frames[i].size.y) / 2.f
                            };

                            if (metadata->Align == AnimationMetadata::Alignment::HorizontalCenter)
                                origin->y = 0;

                            if (metadata->Align == AnimationMetadata::Alignment::VerticalCenter)
                                origin->x = 0;
                        }
                    }

                    animation->AddFrame(Gx::Animation::Frame
                    {
                        frame,
                        origin,
                        position == animation->GetPosition() ? std::nullopt : std::optional(position),
                        std::nullopt,
                        std::nullopt
                    });
                }
            }

            if (metadata->Duration == sf::Time::Zero)
                animation->SetDuration(sf::milliseconds(sheet->Frames.size() * 60));
            else
                animation->SetDuration(metadata->Duration);
        }
    }

    animation->SetName(metadata->Name);
    animation->SetLoop(metadata->IsLoop);
    animation->SetSpeed(metadata->Speed);
    animation->SetRepeatCount(metadata->RepeatCount);
    animation->SetBlendMode(metadata->BlendMode);
    animation->SetColor(metadata->Color);
    animation->SetOrigin(metadata->Origin);
    animation->SetScale(metadata->Scale);
    animation->SetRotation(metadata->Rotation);

    return animation;
}

