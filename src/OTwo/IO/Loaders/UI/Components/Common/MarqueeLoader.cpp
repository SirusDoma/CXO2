#include <OTwo/IO/Loaders/UI/Components/Common/MarqueeLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>

Gx::ResourcePtr<Marquee> MarqueeLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    MarqueeMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!LabelLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;
    
    if (const auto speed = attributes.find("speed"); speed != attributes.end())
        metadata.Speed = speed->get<double>();
    else
        metadata.Speed = 30.f;

    if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        metadata.Bounds = sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Marquee> MarqueeLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const MarqueeMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");

    auto marquee = std::make_unique<Marquee>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Find<sf::Font>(*metadata); font)
        marquee->SetFont(*font);
    else
        return nullptr;

    marquee->SetName(metadata->Name);
    marquee->SetSpeed(metadata->Speed);
    marquee->SetCharacterSize(metadata->FontSize);
    marquee->SetColor(metadata->Color);
    marquee->SetOutlineThickness(metadata->OutlineThickness);
    marquee->SetOutlineColor(metadata->OutlineColor);
    marquee->SetString(metadata->String);
    marquee->SetLocalBounds(metadata->Bounds);

    marquee->SetOrigin(metadata->Origin);
    marquee->SetPosition(metadata->Position);
    marquee->SetScale(metadata->Scale);
    marquee->SetRotation(metadata->Rotation);

    return marquee;
}
