#include <O2/Loaders/UI/ListLoader.hpp>
#include <O2/Loaders/TransformLoader.hpp>

ListLoader::ListLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ListLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ListMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    auto attributes = json.at("attributes");

    ParseReferences(json["require"], metadata);
    TransformLoader::ParseTransform(attributes["transform"], metadata);
    ParseList(attributes, metadata);

    return std::make_unique<ListMetadata>(metadata);
}

Gx::ResourcePtr<Gx::List> ListLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ListMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto list = std::make_unique<Gx::List>(
        spec->VerticalCount,   spec->VerticalSpacing,
        spec->HorizontalCount, spec->HorizontalSpacing
    );

    list->SetName(context.Name);
    list->SetOrigin(spec->Origin);
    list->SetPosition(spec->Position);
    list->SetScale(spec->Scale);
    list->SetRotation(spec->Rotation);

    return list;
}

void ListLoader::ParseList(Json attributes, ListMetadata &metadata)
{
    auto vertical = attributes.find("vertical");
    if (vertical != attributes.end())
    {
        metadata.VerticalCount   = vertical->at("count").get<unsigned int>();
        metadata.VerticalSpacing = vertical->at("spacing").get<float>();
    }
    else
    {
        metadata.VerticalCount   = 1;
        metadata.VerticalSpacing = 0.f;
    }

    auto horizontal = attributes.find("horizontal");
    if (horizontal != attributes.end())
    {
        metadata.HorizontalCount   = horizontal->at("count").get<unsigned int>();
        metadata.HorizontalSpacing = horizontal->at("spacing").get<float>();
    }
    else
    {
        metadata.HorizontalCount   = 1;
        metadata.HorizontalSpacing = 0.f;
    }
}

