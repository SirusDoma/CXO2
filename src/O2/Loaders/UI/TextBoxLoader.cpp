#include <O2/Loaders/UI/TextBoxLoader.hpp>

#include <O2/Loaders/UI/LabelLoader.hpp>
#include <O2/Metadata/UI/TextBoxMetadata.hpp>

TextBoxLoader::TextBoxLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> TextBoxLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    TextBoxMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());

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
        metadata.SetBounds(sf::FloatRect(x, y, w, h));
    }

    auto maxLength = attributes.find("maximumLength");
    if (maxLength != attributes.end())
        metadata.SetMaximumLength(maxLength->get<unsigned int>());
    else
        metadata.SetMaximumLength(0);

    return std::make_unique<TextBoxMetadata>(metadata);
}

Gx::ResourcePtr<Gx::TextBox> TextBoxLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const TextBoxMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    if (!context.Font)
        return nullptr;

    auto textBox = std::make_unique<Gx::TextBox>(spec->GetString(), *context.Font, spec->GetFontSize(), spec->GetBounds());
    textBox->SetColor(spec->GetColor());
    textBox->SetOutlineThickness(spec->GetOutlineThickness());
    textBox->SetOutlineColor(spec->GetOutlineColor());
    textBox->SetMaximumTextLength(spec->GetMaximumLength());

    textBox->SetOrigin(spec->GetOrigin());
    textBox->SetPosition(spec->GetPosition());
    textBox->SetScale(spec->GetScale());
    textBox->SetRotation(spec->GetRotation());

    return textBox;
}
