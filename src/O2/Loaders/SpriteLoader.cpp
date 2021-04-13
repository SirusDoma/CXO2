#include <O2/Loaders/SpriteLoader.hpp>

#include <O2/Loaders/TransformLoader.hpp>
#include <O2/Metadata/SpriteMetadata.hpp>

SpriteLoader::SpriteLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> SpriteLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    SpriteMetadata metadata;

    metadata.SetResourceType(json.at("type").get<std::string>());
    ParseReferences(json["require"], metadata);
    ParseSprite(json["attributes"], metadata);

    return std::make_unique<SpriteMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Sprite> SpriteLoader::Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context) const
{
    auto spec = dynamic_cast<const SpriteMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto sprite = std::make_unique<Gx::Sprite>();
    sprite->SetName(context.Name);
    sprite->SetTexCoords(spec->GetTexCoords());
    sprite->SetColor(spec->GetColor());

    if (context.Texture)
        sprite->SetTexture(*context.Texture);

    sprite->SetOrigin(spec->GetOrigin());
    sprite->SetPosition(spec->GetPosition());
    sprite->SetScale(spec->GetScale());
    sprite->SetRotation(spec->GetRotation());

    return sprite;
}

void SpriteLoader::ParseSprite(Json attributes, SpriteMetadata &metadata)
{
    if (attributes.empty())
        return;

    TransformLoader::ParseTransform(attributes["transform"], metadata);
    auto color = attributes.find("color");
    if (color != attributes.end())
    {
        unsigned int a, r, g, b;
        color->at("a").get_to(a);
        color->at("r").get_to(r);
        color->at("g").get_to(g);
        color->at("b").get_to(b);
        metadata.SetColor(sf::Color(r, g, b, a));
    }
    else
        metadata.SetColor(sf::Color::White);

    auto texCoords  = attributes.find("texCoords");
    if (texCoords != attributes.end())
    {
        unsigned int x, y, w, h;
        texCoords->at("x").get_to(x);
        texCoords->at("y").get_to(y);
        texCoords->at("width").get_to(w);
        texCoords->at("height").get_to(h);
        metadata.SetTexCoords(sf::IntRect(x, y, w, h));
    }
}
