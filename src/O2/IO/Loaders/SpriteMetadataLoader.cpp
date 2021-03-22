#include <O2/IO/Loaders/SpriteMetadataLoader.hpp>
#include <O2/IO/Loaders/TransformMetadataLoader.hpp>

SpriteMetadataLoader::SpriteMetadataLoader()
{
}

Gx::ResourceMetadata* SpriteMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    SpriteMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    SpriteMetadataLoader::Parse(attributes, &metadata);

    return new SpriteMetadata(metadata);
}

void SpriteMetadataLoader::Parse(Json attributes, SpriteMetadata *metadata)
{
    if (attributes.empty())
        return;

    TransformMetadataLoader::Parse(attributes["transform"], metadata);

    auto color = attributes.find("color");
    if (color != attributes.end())
    {
        unsigned int a, r, g, b;
        color->at("a").get_to(a);
        color->at("r").get_to(r);
        color->at("g").get_to(g);
        color->at("b").get_to(b);
        metadata->Color = sf::Color(r, g, b, a);
    }
    else
        metadata->Color = sf::Color::White;

    auto texCoords  = attributes.find("texCoords");
    if (texCoords != attributes.end())
    {
        unsigned int x, y, w, h;
        texCoords->at("x").get_to(x);
        texCoords->at("y").get_to(y);
        texCoords->at("width").get_to(w);
        texCoords->at("height").get_to(h);
        metadata->TexCoords = sf::IntRect(x, y, w, h);
    }
}

Gx::Sprite* SpriteMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<SpriteMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto sprite = new Gx::Sprite();
    sprite->SetName(spec->Name);
    sprite->SetTexCoords(spec->TexCoords);
    sprite->SetColor(spec->Color);

    if (context.Texture)
        sprite->SetTexture(context.Texture);

    sprite->SetOrigin(spec->Origin);
    sprite->SetPosition(spec->Position);
    sprite->SetScale(spec->Scale);
    sprite->SetRotation(spec->Rotation);

    return sprite;
}
