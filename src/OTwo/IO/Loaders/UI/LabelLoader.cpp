#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/UI/LabelMetadata.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    LabelMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!ParseMetadata(attributes, metadata, context))
        return nullptr;
    
    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const LabelMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto label = std::make_unique<Gx::Label>();
    auto ctx = ResourceContextDecorator::Decorate(context);
    if (auto font = ctx.Find<sf::Font>(*metadata); font)
        label->SetFont(*font);

    label->SetCharacterSize(metadata->FontSize);
    label->SetColor(metadata->Color);
    label->SetOutlineThickness(metadata->OutlineThickness);
    label->SetOutlineColor(metadata->OutlineColor);
    label->SetString(metadata->String);
    label->SetOrigin(metadata->Origin);
    label->SetPosition(metadata->Position);
    label->SetScale(metadata->Scale);
    label->SetRotation(metadata->Rotation);
    label->SetAlignment(metadata->Alignment);

    auto populator = ObjectPopulator::Decorate(label.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return label;
}

bool LabelLoader::ParseMetadata(Gx::Json attributes, LabelMetadata& metadata, const Gx::ResourceContext &context)
{
    if (attributes.empty())
        return false;

    if (!TransformLoader::ParseMetadata(attributes.at("transform"), metadata))
        return false;

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

    auto alignment = attributes.find("alignment");
    if (alignment != attributes.end())
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
