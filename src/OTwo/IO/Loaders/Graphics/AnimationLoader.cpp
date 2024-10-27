#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/Metadata/Graphics/AnimationMetadata.hpp>

Gx::ResourcePtr<Gx::Animation> AnimationLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    AnimationMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

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
                origin = std::nullopt;

            metadata.Frames.push_back(Gx::Animation::Frame
            {
                frameMetadata.TexCoords,
                origin,
                position,
                rotation,
                scale
            });
        }
    }
    else
    {
        metadata.Frames.push_back(Gx::Animation::Frame
        {
            metadata.TexCoords,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt
        });
    }

    const auto loop = attributes.find("isLoop");
    metadata.IsLoop = loop != attributes.end() && loop->get<bool>();

    const auto repeatCount = attributes.find("repeatCount");
    metadata.RepeatCount = repeatCount != attributes.end() ? repeatCount->get<unsigned int>() : 0;

    const auto speed = attributes.find("speed");
    metadata.Speed = speed != attributes.end() ? speed->get<float>() : 1.0f;

    if (const auto duration = attributes.find("duration"); duration != attributes.end())
        metadata.Duration = sf::milliseconds(duration->get<unsigned int>());
    else
        metadata.Duration = sf::milliseconds(metadata.Frames.size() * 60);

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Animation> AnimationLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const AnimationMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto animation = std::make_unique<Gx::Animation>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        animation->SetTexture(*texture);

    for (const auto& frame : metadata->Frames)
        animation->AddFrame(frame);

    if (metadata->TexCoords != sf::IntRect())
        animation->SetTexCoords(metadata->TexCoords);

    animation->SetName(metadata->Name);
    animation->SetLoop(metadata->IsLoop);
    animation->SetDuration(metadata->Duration);
    animation->SetSpeed(metadata->Speed);
    animation->SetRepeatCount(metadata->RepeatCount);
    animation->SetBlendMode(metadata->BlendMode);
    animation->SetColor(metadata->Color);
    animation->SetOrigin(metadata->Origin);
    animation->SetPosition(metadata->Position);
    animation->SetScale(metadata->Scale);
    animation->SetRotation(metadata->Rotation);

    return animation;
}

