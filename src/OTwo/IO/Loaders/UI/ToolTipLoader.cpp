#include <OTwo/IO/Loaders/UI/ToolTipLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::ToolTip> ToolTipLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    ToolTipMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!LabelLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;
    
    if (!attributes["duration"].empty())
        metadata.Duration = sf::milliseconds(attributes["duration"].get<unsigned int>());
    else
        metadata.Duration = sf::seconds(3.f);

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
        metadata.Color = sf::Color::Black;

    if (auto container = attributes.find("container"); container != attributes.end())
    {
        if (auto padding = container->find("padding"); padding != container->end())
        {
            float x, y;
            padding->at("x").get_to(x);
            padding->at("y").get_to(y);

            metadata.Padding = sf::Vector2f(x, y);
        }
        else
            metadata.Padding = sf::Vector2f(metadata.FontSize * (10.f / 100.f), metadata.FontSize * (10.f / 100.f));

        color = container->find("color");
        if (color != container->end())
        {
            unsigned int a, r, g, b;
            color->at("a").get_to(a);
            color->at("r").get_to(r);
            color->at("g").get_to(g);
            color->at("b").get_to(b);
            metadata.ContainerFillColor = sf::Color(r, g, b, a);
        }
        else
            metadata.ContainerFillColor = sf::Color::White;

        if (auto outline = container->find("outline"); outline != container->end())
        {
            if (auto thickness = outline->find("thickness"); thickness != outline->end())
                metadata.ContainerOutlineThickness = thickness->get<float>();
            else
                metadata.ContainerOutlineThickness = 1.f;

            color = outline->find("color");
            if (color != outline->end())
            {
                unsigned int a, r, g, b;
                color->at("a").get_to(a);
                color->at("r").get_to(r);
                color->at("g").get_to(g);
                color->at("b").get_to(b);
                metadata.ContainerOutlineColor = sf::Color(r, g, b, a);
            }
            else
                metadata.ContainerOutlineColor = sf::Color::Black;
        }
        else
        {
            metadata.ContainerOutlineThickness = 1.f;
            metadata.ContainerOutlineColor = sf::Color::Black;
        }
    }
    else
    {
        metadata.Padding = sf::Vector2f(metadata.FontSize * (10.f / 100.f), metadata.FontSize * (10.f / 100.f));
        metadata.ContainerFillColor = sf::Color::White;
        metadata.ContainerOutlineThickness = 1.f;
        metadata.ContainerOutlineColor = sf::Color::Black;
    }
    
    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::ToolTip> ToolTipLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ToolTipMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto toolTip = std::make_unique<Gx::ToolTip>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Find<Gx::Font>(*metadata); font)
        toolTip->SetFont(*font);
    else
        return nullptr;

    toolTip->Hide();
    
    toolTip->SetName(metadata->Name);
    toolTip->SetDuration(metadata->Duration);
    
    toolTip->SetCharacterSize(metadata->FontSize);
    toolTip->SetColor(metadata->Color);
    toolTip->SetPadding(metadata->Padding);
    toolTip->SetContainerColor(metadata->ContainerFillColor);
    toolTip->SetContainerOutlineColor(metadata->ContainerOutlineColor);
    toolTip->SetContainerOutlineThickness(metadata->ContainerOutlineThickness);
    toolTip->SetOutlineThickness(metadata->OutlineThickness);
    toolTip->SetOutlineColor(metadata->OutlineColor);
    toolTip->SetString(metadata->String);

    toolTip->SetOrigin(metadata->Origin);
    toolTip->SetPosition(metadata->Position);
    toolTip->SetScale(metadata->Scale);
    toolTip->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(toolTip.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(context, name);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return toolTip;
}
