#include <OTwo/IO/Loaders/UI/GaugeLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <OTwo/Metadata/UI/GaugeMetadata.hpp>

#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>

Gx::ResourcePtr<Gx::Gauge> GaugeLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    GaugeMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto orientation = attributes.find("orientation"); orientation != attributes.end())
    {
        if (orientation->get<std::string>() == "VERTICAL")
            metadata.Orientation = Gx::Gauge::Orientation::Vertical;
        else
            metadata.Orientation = Gx::Gauge::Orientation::Horizontal;
    }

    if (const auto maximum = attributes.find("maximum"); maximum != attributes.end())
        metadata.Maximum = maximum->get<float>();
    else
        metadata.Maximum = 100.0f;

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

            metadata.AnimationFrames.push_back(Gx::Animation::Frame
            {
                frameMetadata.TexCoords,
                origin,
                position,
                rotation,
                scale
            });
        }
    }

    if (const auto duration = attributes.find("duration"); duration != attributes.end())
        metadata.AnimationDuration = sf::milliseconds(duration->get<unsigned int>());
    else
        metadata.AnimationDuration = sf::milliseconds(metadata.AnimationFrames.size() * 60);

    if (const auto flicker = attributes.find("flicker"); flicker != attributes.end())
        metadata.Flicker = flicker->get<bool>();
    else
        metadata.Flicker = false;


    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Gauge> GaugeLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const GaugeMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto gauge = std::make_unique<Gx::Gauge>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        gauge->SetTexture(*texture);

    gauge->SetFlickering(metadata->Flicker);
    gauge->SetAnimationDuration(metadata->AnimationDuration);
    for (const auto& frame : metadata->AnimationFrames)
        gauge->AddAnimationFrame(frame);
    
    gauge->SetName(metadata->Name);
    gauge->SetOrientation(metadata->Orientation);
    gauge->SetTexCoords(metadata->TexCoords);
    gauge->SetMaximumValue(metadata->Maximum);
    gauge->SetColor(metadata->Color);
    gauge->SetOrigin(metadata->Origin);
    gauge->SetPosition(metadata->Position);
    gauge->SetScale(metadata->Scale);
    gauge->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(gauge.get());
    LoadChildren(container, meta, context);

    return gauge;
}
