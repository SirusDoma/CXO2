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
    TransformMetadataLoader::Parse(attributes["transform"], metadata);

    attributes.at("fontSize").get_to(metadata->FontSize);
    attributes.at("string").get_to(metadata->String);
}

Gx::Label* LabelMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<LabelMetadata*>(metadata);
    if (!spec)
        return nullptr;

    return new Gx::Label(spec->String, context.Font, spec->FontSize);
}
