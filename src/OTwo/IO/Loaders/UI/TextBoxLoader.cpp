#include <OTwo/IO/Loaders/UI/TextBoxLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/TextBoxMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::TextBox> TextBoxLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    TextBoxMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!LabelLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        
        metadata.Bounds = sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
    }

    if (const auto maxLength = attributes.find("maximumLength"); maxLength != attributes.end())
        metadata.MaximumLength = maxLength->get<unsigned int>();
    else
        metadata.MaximumLength = 0;

    if (const auto highlightTextColor = attributes.find("highlightTextColor"); highlightTextColor != attributes.end())
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

    if (const auto highlightBackColor = attributes.find("highlightBackColor"); highlightBackColor != attributes.end())
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

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::TextBox> TextBoxLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const TextBoxMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto textBox = std::make_unique<Gx::TextBox>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Find<Gx::Font>(*metadata); font)
        textBox->SetFont(*font);

    textBox->SetCharacterSize(metadata->FontSize);
    textBox->SetColor(metadata->Color);
    textBox->SetOutlineThickness(metadata->OutlineThickness);
    textBox->SetOutlineColor(metadata->OutlineColor);
    textBox->SetString(metadata->String);

    textBox->SetLocalBounds(metadata->Bounds);
    textBox->SetMaximumTextLength(metadata->MaximumLength);
    textBox->SetHighlightTextColor(metadata->HighlightTextColor);
    textBox->SetHighlightBackColor(metadata->HighlightBackColor);

    textBox->SetOrigin(metadata->Origin);
    textBox->SetPosition(metadata->Position);
    textBox->SetScale(metadata->Scale);
    textBox->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(textBox.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return textBox;
}
