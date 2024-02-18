#include <OTwo/Loaders/UI/TextBoxLoader.hpp>

#include <OTwo/Loaders/UI/LabelLoader.hpp>
#include <OTwo/Metadata/UI/TextBoxMetadata.hpp>

TextBoxLoader::TextBoxLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> TextBoxLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    TextBoxMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json.at("attributes");
    ParseReferences(json["require"], metadata);
    LabelLoader::ParseLabel(attributes, metadata);

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

    auto maxLength = attributes.find("maximumLength");
    if (maxLength != attributes.end())
        metadata.MaximumLength = maxLength->get<unsigned int>();
    else
        metadata.MaximumLength = 0;

    auto highlightTextColor = attributes.find("highlightTextColor");
    if (highlightTextColor != attributes.end())
    {
        unsigned int a, r, g, b;
        highlightTextColor->at("a").get_to(a);
        highlightTextColor->at("r").get_to(r);
        highlightTextColor->at("g").get_to(g);
        highlightTextColor->at("b").get_to(b);
        metadata.HighlightTextColor = sf::Color(r, g, b, a);
    }
    else
        metadata.HighlightTextColor = sf::Color::Black;

    auto highlightBackColor = attributes.find("highlightBackColor");
    if (highlightBackColor != attributes.end())
    {
        unsigned int a, r, g, b;
        highlightBackColor->at("a").get_to(a);
        highlightBackColor->at("r").get_to(r);
        highlightBackColor->at("g").get_to(g);
        highlightBackColor->at("b").get_to(b);
        metadata.HighlightBackColor = sf::Color(r, g, b, a);
    }
    else
        metadata.HighlightBackColor = sf::Color::White;

    return std::make_unique<TextBoxMetadata>(metadata);
}

Gx::ResourcePtr<Gx::TextBox> TextBoxLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const TextBoxMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    if (!context.Font)
        return nullptr;

    auto textBox = std::make_unique<Gx::TextBox>(spec->String, *context.Font, spec->FontSize, spec->Bounds);
    textBox->SetColor(spec->Color);
    textBox->SetHighlightTextColor(spec->HighlightTextColor);
    textBox->SetHighlightBackColor(spec->HighlightBackColor);
    textBox->SetOutlineThickness(spec->OutlineThickness);
    textBox->SetOutlineColor(spec->OutlineColor);
    textBox->SetMaximumTextLength(spec->MaximumLength);

    textBox->SetOrigin(spec->Origin);
    textBox->SetPosition(spec->Position);
    textBox->SetScale(spec->Scale);
    textBox->SetRotation(spec->Rotation);

    return textBox;
}
