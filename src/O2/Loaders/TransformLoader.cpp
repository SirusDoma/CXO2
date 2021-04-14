#include <O2/Loaders/TransformLoader.hpp>
#include <O2/Metadata/TransformMetadata.hpp>

TransformLoader::TransformLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> TransformLoader::LoadMetadata(const void* data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));

    auto attributes = json.at("attributes");
    TransformMetadata metadata = TransformMetadata();
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    return std::make_unique<TransformMetadata>(metadata);
}

Gx::ResourcePtr<sf::Transform> TransformLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const TransformMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto transform = sf::Transform();
    transform.translate(spec->Position);
    transform.scale(spec->Scale);
    transform.rotate(spec->Rotation);

    return std::make_unique<sf::Transform>(transform);
}

void TransformLoader::ParseTransform(Json attributes, TransformMetadata &metadata)
{
    auto name = attributes.find("name");
    if (name != attributes.end())
        metadata.Name = name->get<std::string>();

    auto p = attributes.find("position");
    auto position = sf::Vector2f();
    if (p != attributes.end())
    {
        p->at("x").get_to(position.x);
        p->at("y").get_to(position.y);
    }
    metadata.Position = position;

    auto s = attributes.find("scale");
    auto scale = sf::Vector2f(1.f, 1.f);
    if (s != attributes.end())
    {
        s->at("scale").at("x").get_to(scale.x);
        s->at("scale").at("y").get_to(scale.y);
    }
    metadata.Scale = scale;

    auto r = attributes.find("rotation");
    float rotation = 0;
    if (r != attributes.end())
        r->get_to(rotation);
    metadata.Rotation = rotation;

    auto o  = attributes.find("origin");
    auto origin = sf::Vector2f();
    if (o != attributes.end())
    {
        o->at("x").get_to(origin.x);
        o->at("y").get_to(origin.y);
    }
    metadata.Origin = origin;
}


