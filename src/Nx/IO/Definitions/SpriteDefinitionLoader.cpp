#include <Nx/IO/Definitions/SpriteDefinitionLoader.hpp>

SpriteDefinitionLoader::SpriteDefinitionLoader()
{
}

Gx::ResourceDefinition* SpriteDefinitionLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    SpriteDefinition definition;

    json.at("type").get_to(definition.Type);
    json.at("texture").get_to(definition.Texture);

    auto properties = json.at("data");
    auto color      = properties.at("color");
    auto texCoords  = properties.at("texCoords");

    unsigned int a, r, g, b;
    color.at("a").get_to(a);
    color.at("r").get_to(r);
    color.at("g").get_to(g);
    color.at("b").get_to(b);
    definition.Color = sf::Color(r, g, b, a);

    unsigned int x, y, w, h;
    texCoords.at("x").get_to(x);
    texCoords.at("y").get_to(y);
    texCoords.at("width").get_to(w);
    texCoords.at("height").get_to(h);
    definition.TexCoords = sf::IntRect(x, y, w, h);

    return new SpriteDefinition(definition);
}

Gx::Sprite* SpriteDefinitionLoader::Create(Gx::ResourceDefinition* definition, Gx::ResourceContext context) const
{
    auto sprite = new Gx::Sprite();
    auto spec   = dynamic_cast<SpriteDefinition*>(definition);
    
    if (!spec)
        return nullptr;

    sprite->SetTexture(context.Texture);
    sprite->SetTexCoords(spec->TexCoords);
    sprite->SetColor(spec->Color);

    return sprite;
}