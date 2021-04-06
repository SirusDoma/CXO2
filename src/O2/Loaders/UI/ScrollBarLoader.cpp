#include <O2/Loaders/UI/ScrollBarLoader.hpp>

#include <O2/Metadata/UI/ScrollBarMetadata.hpp>
#include <O2/Loaders/SpriteLoader.hpp>

ScrollBarLoader::ScrollBarLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ScrollBarLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ScrollBarMetadata metadata;

    auto attributes = json.at("attributes");
    metadata.SetType(json.at("type").get<std::string>());

    ScrollBarLoader::ParseReferences(json["require"], metadata);
    SpriteLoader::ParseSprite(attributes, metadata);

    auto orientation = attributes.find("orientation");
    if (orientation != attributes.end())
    {
        if (orientation->get<std::string>() == "VERTICAL")
            metadata.SetOrientation(Gx::ScrollBar::Vertical);
        else
            metadata.SetOrientation(Gx::ScrollBar::Horizontal);
    }

    auto maximum = attributes.find("maximum");
    if (maximum != attributes.end())
        metadata.SetMaximum(maximum->get<float>());
    else
        metadata.SetMaximum(100.0f);

    auto step = attributes.find("step");
    if (step != attributes.end())
        metadata.SetStep(step->get<float>());
    else
        metadata.SetStep(1.0f);

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

    return std::make_unique<ScrollBarMetadata>(metadata);
}

Gx::ResourcePtr<Gx::ScrollBar> ScrollBarLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ScrollBarMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    if (!context.Texture)
        return nullptr;

    auto scrollBar = std::make_unique<Gx::ScrollBar>(*context.Texture, spec->GetTexCoords(), spec->GetBounds(), spec->GetOrientation());
    scrollBar->SetColor(spec->GetColor());
    scrollBar->SetMaximumValue(spec->GetMaximum());
    scrollBar->SetStep(spec->GetStep());

    scrollBar->SetOrigin(spec->GetOrigin());
    scrollBar->SetPosition(spec->GetPosition());
    scrollBar->SetScale(spec->GetScale());
    scrollBar->SetRotation(spec->GetRotation());

    return scrollBar;
}
