#include <OTwo/Loaders/UI/Components/MarqueeLoader.hpp>
#include <OTwo/Loaders/UI/LabelLoader.hpp>

MarqueeLoader::MarqueeLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> MarqueeLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    MarqueeMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json.at("attributes");
    ParseReferences(json["require"], metadata);
    LabelLoader::ParseLabel(attributes, metadata);

    metadata.Speed = attributes.at("speed").get<double>();
    auto bounds = attributes.find("bounds");
    if (bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        metadata.Bounds = sf::FloatRect(x, y, w, h);
    }

    return std::make_unique<MarqueeMetadata>(metadata);
}

Gx::ResourcePtr<Marquee> MarqueeLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const MarqueeMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto marquee = std::make_unique<Marquee>();
    if (context.Font)
        marquee->SetFont(*context.Font);

    marquee->SetCharacterSize(spec->FontSize);
    marquee->SetColor(spec->Color);
    marquee->SetOutlineThickness(spec->OutlineThickness);
    marquee->SetOutlineColor(spec->OutlineColor);
    marquee->SetString(spec->String);

    marquee->SetOrigin(spec->Origin);
    marquee->SetPosition(spec->Position);
    marquee->SetScale(spec->Scale);
    marquee->SetRotation(spec->Rotation);

    marquee->SetSpeed(spec->Speed);
    marquee->SetLocalBounds(spec->Bounds);

    return marquee;
}
