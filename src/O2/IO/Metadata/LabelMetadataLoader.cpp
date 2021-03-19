#include <O2/IO/Metadata/LabelMetadataLoader.hpp>

LabelMetadataLoader::LabelMetadataLoader()
{
}

Gx::ResourceMetadata* LabelMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    LabelMetadata metadata;
    Parse(json, &metadata);

    return new LabelMetadata(metadata);
}

void LabelMetadataLoader::Parse(Json json, LabelMetadata *metadata)
{
    if (json.empty())
        return;

    json.at("type").get_to(metadata->Type);
    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata->ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    attributes.at("fontSize").get_to(metadata->FontSize);
    TransformMetadataLoader::Parse(attributes["transform"], metadata);

    auto string = attributes.find("string");
    if (string != attributes.end())
        string->get_to(metadata->String);
}

Gx::Label* LabelMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<LabelMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto label = new Gx::Label(spec->String, context.Font, spec->FontSize);
    label->SetOrigin(spec->Origin);
    label->SetPosition(spec->Position);
    label->SetScale(spec->Scale);
    label->SetRotation(spec->Rotation);

    return label;
}
