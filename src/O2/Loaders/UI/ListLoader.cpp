#include <O2/Loaders/UI/ListLoader.hpp>
#include <O2/Loaders/TransformLoader.hpp>

ListLoader::ListLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ListLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ListMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());
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
        spec->GetVerticalCount(),   spec->GetVerticalSpacing(),
        spec->GetHorizontalCount(), spec->GetHorizontalSpacing()
    );

    list->SetName(context.Name);
    list->SetOrigin(spec->GetOrigin());
    list->SetPosition(spec->GetPosition());
    list->SetScale(spec->GetScale());
    list->SetRotation(spec->GetRotation());

    return list;
}

void ListLoader::ParseList(Json attributes, ListMetadata &metadata)
{
    auto vertical = attributes.find("vertical");
    if (vertical != attributes.end())
    {
        metadata.SetVerticalCount(vertical->at("count").get<unsigned int>());
        metadata.SetVerticalSpacing(vertical->at("spacing").get<float>());
    }
    else
    {
        metadata.SetVerticalCount(1);
        metadata.SetVerticalSpacing(0.f);
    }

    auto horizontal = attributes.find("horizontal");
    if (horizontal != attributes.end())
    {
        metadata.SetHorizontalCount(horizontal->at("count").get<unsigned int>());
        metadata.SetHorizontalSpacing(horizontal->at("spacing").get<float>());
    }
    else
    {
        metadata.SetHorizontalCount(1);
        metadata.SetHorizontalSpacing(0.f);
    }
}

