#include <O2/IO/Metadata/TransformMetadataLoader.hpp>

TransformMetadataLoader::TransformMetadataLoader()
{
}

Gx::ResourceMetadata* TransformMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));

    auto attributes = json.at("attributes");
    TransformMetadata metadata = TransformMetadata();
    TransformMetadataLoader::Parse(attributes["transform"], &metadata);

    return new TransformMetadata(metadata);
}


void TransformMetadataLoader::Parse(Json attributes, TransformMetadata *metadata)
{
    auto position = attributes.find("position");
    if (position != attributes.end())
    {
        position->at("x").get_to(metadata->Position.x);
        position->at("y").get_to(metadata->Position.y);
    }
    else
        metadata->Position = sf::Vector2f();

    auto scale = attributes.find("scale");
    if (scale != attributes.end())
    {
        scale->at("scale").at("x").get_to(metadata->Scale.x);
        scale->at("scale").at("y").get_to(metadata->Scale.y);
    }
    else
        metadata->Scale = sf::Vector2f(1.f, 1.f); // default scale

    auto rotation = attributes.find("rotation");
    if (rotation != attributes.end())
        rotation->get_to(metadata->Rotation);
    else
        metadata->Rotation = 0;

    auto origin  = attributes.find("origin");
    if (origin != attributes.end())
    {
        float x, y;
        origin->at("x").get_to(x);
        origin->at("y").get_to(y);
        metadata->Origin = sf::Vector2f (x, y);
    }
    else
        metadata->Origin = sf::Vector2f();
}

sf::Transform* TransformMetadataLoader::Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<TransformMetadata*>(definition);
    if (!spec)
        return nullptr;

    auto transform = new sf::Transform();
    transform->translate(spec->Position);
    transform->scale(spec->Scale);
    transform->rotate(spec->Rotation);

    return transform;
}

