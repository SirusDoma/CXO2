#include <OTwo/Loaders/UI/LabelLoader.hpp>

#include <OTwo/Metadata/UI/LabelMetadata.hpp>
#include <OTwo/Loaders/TransformLoader.hpp>

LabelLoader::LabelLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> LabelLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    LabelMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json.at("attributes");
    ParseReferences(json["require"], metadata);
    ParseLabel(attributes, metadata);

    return std::make_unique<LabelMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Label> LabelLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const LabelMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto label = std::make_unique<Gx::Label>();
    if (context.Font)
        label->SetFont(*context.Font);

    label->SetCharacterSize(spec->FontSize);
    label->SetColor(spec->Color);
    label->SetOutlineThickness(spec->OutlineThickness);
    label->SetOutlineColor(spec->OutlineColor);
    label->SetString(spec->String);

    label->SetOrigin(spec->Origin);
    label->SetPosition(spec->Position);
    label->SetScale(spec->Scale);
    label->SetRotation(spec->Rotation);

    return label;
}

void LabelLoader::ParseLabel(Json attributes, LabelMetadata &metadata)
{
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto fontSize = attributes.find("fontSize");
    if (fontSize != attributes.end())
        metadata.FontSize = fontSize->get<unsigned int>();
    else
        metadata.FontSize = 30;

    auto string = attributes.find("string");
    if (string != attributes.end())
        metadata.String = string->get<std::string>();

    auto color = attributes.find("color");
    if (color != attributes.end())
    {
        unsigned int a, r, g, b;
        color->at("a").get_to(a);
        color->at("r").get_to(r);
        color->at("g").get_to(g);
        color->at("b").get_to(b);
        metadata.Color = sf::Color(r, g, b, a);
    }
    else
        metadata.Color = sf::Color::White;

    auto outline = attributes.find("outline");
    if (outline != attributes.end())
    {
        auto thickness = outline->find("thickness");
        if (thickness != outline->end())
            metadata.OutlineThickness = thickness->get<float>();

        color = outline->find("color");
        if (color != outline->end())
        {
            unsigned int a, r, g, b;
            color->at("a").get_to(a);
            color->at("r").get_to(r);
            color->at("g").get_to(g);
            color->at("b").get_to(b);
            metadata.OutlineColor = sf::Color(r, g, b, a);
        }
    }
    else
    {
        metadata.OutlineThickness = 0.f;
        metadata.OutlineColor = sf::Color::Transparent;
    }
}
