#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/LabelMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    LabelMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return Instantiate(context);

    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (!ParseMetadata(attributes.value(), metadata, context))
            return Instantiate(context);
    }
    
    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const LabelMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    auto label = Instantiate(context);
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Require<Gx::Font>(*metadata); font)
        label->SetFont(*font);

    if (const auto ffIt = metadata->Require.find("fallbackFonts"); ffIt != metadata->Require.end() && ffIt->second.has_value())
    {
        auto ffResource = ffIt->second;
        auto fallbackFonts = std::vector<std::string>();

        if (ffResource.type() == typeid(Gx::Json))
            fallbackFonts = std::any_cast<Gx::Json>(ffResource);
        else if (ffResource.type() == typeid(std::vector<std::string>()))
            fallbackFonts = std::any_cast<std::vector<std::string>>(ffResource);

        for (const auto& fontPath : fallbackFonts)
        {
            if (const auto font = ctx.Find<Gx::Font>(fontPath))
                label->AddFallbackFont(*font);
            else if (Gx::FileSystem::Contains(fontPath))
                label->AddFallbackFont(ctx.Acquire<Gx::Font>(fontPath));
        }
    }

    if (metadata->Position != sf::Vector2f())
    {
        label->SetPosition(metadata->Position);
    }
    else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
    {
        label->SetPosition(sf::Vector2f(
            static_cast<float>(bound->position.x),
            static_cast<float>(bound->position.y)
        ));
    }

    std::uint32_t style = 0;
    if (metadata->Bold)
        style |= Gx::Label::Style::Bold;

    if (metadata->Italic)
        style |= Gx::Label::Style::Italic;

    if (metadata->Underlined)
        style |= Gx::Label::Style::Underlined;

    label->SetName(metadata->Name);
    label->SetStyle(style);
    label->SetCharacterSize(metadata->FontSize);
    label->SetCharacterWidth(metadata->FontWidth);
    label->SetColor(metadata->Color);
    label->SetOutlineColor(metadata->OutlineColor);
    label->SetOutlineThickness(metadata->OutlineThickness);
    label->SetOutlineOffset({0.f, 1.f});
    label->SetString(sf::String::fromUtf8(metadata->String.begin(), metadata->String.end()));
    label->SetOrigin(metadata->Origin);
    label->SetScale(metadata->Scale);
    label->SetRotation(metadata->Rotation);
    label->SetAlignment(metadata->Alignment);

    if (metadata->Kerning > 0)
        label->SetLetterSpacing(metadata->Kerning);

    auto container = ObjectContainer::Decorate(label.get());
    LoadChildren(container, meta, context);

    return label;
}

bool LabelLoader::ParseMetadata(const Gx::Json& attributes, LabelMetadata& metadata, const Gx::ResourceContext& context)
{
    if (attributes.empty())
        return false;

    if (const auto transform = attributes.find("transform"); transform != attributes.end())
    {
        if (!TransformLoader::ParseMetadata(transform.value(), metadata))
            return false;
    }

    metadata.FontSize = 30;
    if (auto fontSize = attributes.find("fontSize"); fontSize != attributes.end())
        metadata.FontSize = fontSize->get<unsigned int>();

    metadata.FontWidth = 0;
    if (auto fontWidth = attributes.find("fontWidth"); fontWidth != attributes.end())
        metadata.FontWidth = fontWidth->get<unsigned int>();

    if (auto string = attributes.find("string"); string != attributes.end())
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

    metadata.Bold = false;
    if (auto bold = attributes.find("bold"); bold != attributes.end())
        metadata.Bold = bold->get<bool>();

    metadata.Italic = false;
    if (auto italic = attributes.find("italic"); italic != attributes.end())
        metadata.Italic = italic->get<bool>();

    metadata.Underlined = false;
    if (auto underlined = attributes.find("underlined"); underlined != attributes.end())
        metadata.Underlined = underlined->get<bool>();

    metadata.Kerning = 0;
    if (auto spacing = attributes.find("kerning"); spacing != attributes.end())
        metadata.Kerning = spacing->get<float>();

    if (auto outline = attributes.find("outline"); outline != attributes.end())
    {
        if (auto thickness = outline->find("thickness"); thickness != outline->end())
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

    if (auto alignment = attributes.find("alignment"); alignment != attributes.end())
    {
        if (auto parsed = magic_enum::enum_cast<Gx::Label::Alignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
            metadata.Alignment = parsed.value();
        else
            metadata.Alignment = Gx::Label::Alignment::None;
    }
    else
        metadata.Alignment = Gx::Label::Alignment::None;

    return true;
}
