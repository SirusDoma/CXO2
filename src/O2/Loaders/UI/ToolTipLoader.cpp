#include <O2/Loaders/UI/ToolTipLoader.hpp>
#include <O2/Loaders/UI/LabelLoader.hpp>

ToolTipLoader::ToolTipLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ToolTipLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ToolTipMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json.at("attributes");
    ParseReferences(json["require"], metadata);
    LabelLoader::ParseLabel(attributes, metadata);

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

    auto container = attributes.find("container");
    if (container != attributes.end())
    {
        auto padding = container->find("padding");
        if (padding != container->end())
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

        auto outline = container->find("outline");
        if (outline != container->end())
        {
            auto thickness = outline->find("thickness");
            if (thickness != outline->end())
                metadata.ContainerOutlineThickness = thickness->get<float>();

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
        }
        else
        {
            metadata.ContainerOutlineThickness = 1.f;
            metadata.ContainerOutlineColor     = sf::Color::Black;
        }
    }
    else
    {
        metadata.Padding                   = sf::Vector2f(metadata.FontSize * (10.f / 100.f), metadata.FontSize * (10.f / 100.f));
        metadata.ContainerFillColor        = sf::Color::White;
        metadata.ContainerOutlineThickness = 1.f;
        metadata.ContainerOutlineColor     = sf::Color::Black;
    }

    return std::make_unique<ToolTipMetadata>(metadata);
}

Gx::ResourcePtr<Gx::ToolTip> ToolTipLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ToolTipMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto toolTip = std::make_unique<Gx::ToolTip>();
    if (context.Font)
        toolTip->SetFont(*context.Font);

    toolTip->SetCharacterSize(spec->FontSize);
    toolTip->SetColor(spec->Color);
    toolTip->SetOutlineThickness(spec->OutlineThickness);
    toolTip->SetOutlineColor(spec->OutlineColor);
    toolTip->SetString(spec->String);

    toolTip->SetDuration(spec->Duration);
    toolTip->SetPadding(spec->Padding);
    toolTip->SetContainerFillColor(spec->ContainerFillColor);
    toolTip->SetContainerOutlineColor(spec->ContainerOutlineColor);
    toolTip->SetContainerOutlineThickness(spec->ContainerOutlineThickness);
    toolTip->Hide();

    toolTip->SetOrigin(spec->Origin);
    toolTip->SetPosition(spec->Position);
    toolTip->SetScale(spec->Scale);
    toolTip->SetRotation(spec->Rotation);

    return toolTip;
}
