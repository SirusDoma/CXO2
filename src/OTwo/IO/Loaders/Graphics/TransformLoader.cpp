#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

Gx::ResourcePtr<sf::Transform> TransformLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    TransformMetadata metadata;
    MetadataLoader::Parse(json, metadata, ctx);

    auto attributes = json.at("attributes");
    if (const auto data = attributes.find("transform"); data != attributes.end())
        ParseMetadata(data.value(), metadata, ctx);

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<sf::Transform> TransformLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const TransformMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto transform = sf::Transform();
    transform.translate(metadata->Position);
    transform.scale(metadata->Scale);
    transform.rotate(sf::degrees(metadata->Rotation));

    return std::make_unique<sf::Transform>(transform);
}

bool TransformLoader::ParseMetadata(Gx::Json transform, TransformMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (transform.empty())
        return false;

    const auto p = transform.find("position");
    auto position = sf::Vector2f();
    if (p != transform.end())
    {
        p->at("x").get_to(position.x);
        p->at("y").get_to(position.y);
    }
    metadata.Position = position;

    const auto s = transform.find("scale");
    auto scale = sf::Vector2f(1.f, 1.f);
    if (s != transform.end())
    {
        s->at("x").get_to(scale.x);
        s->at("y").get_to(scale.y);
    }
    metadata.Scale = scale;

    const auto r = transform.find("rotation");
    float rotation = 0;
    if (r != transform.end())
        r->get_to(rotation);
    metadata.Rotation = rotation;

    const auto o  = transform.find("origin");
    auto origin = sf::Vector2f();
    if (o != transform.end())
    {
        o->at("x").get_to(origin.x);
        o->at("y").get_to(origin.y);
    }
    metadata.Origin = origin;

    return true;
}
