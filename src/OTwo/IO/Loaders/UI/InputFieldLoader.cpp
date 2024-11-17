#include <OTwo/IO/Loaders/UI/InputFieldLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/InputFieldMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::InputField> InputFieldLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    InputFieldMetadata metadata;
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

Gx::ResourcePtr<Gx::InputField> InputFieldLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const InputFieldMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    auto input = std::make_unique<Gx::InputField>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Find<Gx::Font>(*metadata); font)
        input->SetFont(*font);

    input->SetCharacterSize(metadata->FontSize);
    input->SetColor(metadata->Color);
    input->SetOutlineThickness(metadata->OutlineThickness);
    input->SetOutlineColor(metadata->OutlineColor);
    input->SetString(metadata->String);

    input->SetLocalBounds(metadata->Bounds);
    input->SetMaximumTextLength(metadata->MaximumLength);
    input->SetHighlightTextColor(metadata->HighlightTextColor);
    input->SetHighlightBackColor(metadata->HighlightBackColor);

    input->SetOrigin(metadata->Origin);
    input->SetPosition(metadata->Position);
    input->SetScale(metadata->Scale);
    input->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(input.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return input;
}
