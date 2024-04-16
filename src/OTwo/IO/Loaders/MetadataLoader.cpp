#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<ResourceMetadata> MetadataLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    ResourceMetadata metadata;
    Parse(json, metadata, ctx);

    return std::make_unique<ResourceMetadata>(metadata);
}

Gx::ResourcePtr<ResourceMetadata> MetadataLoader::LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    return std::make_unique<ResourceMetadata>(metadata);
}

bool MetadataLoader::Parse(const Gx::Json &json, ResourceMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (json.empty())
        return false;

    metadata.Name = ctx.GetID();
    if (const auto name = json.find("name"); name != json.end())
        metadata.Name = name->get<std::string>();

    metadata.Type = ResourceMetadata::ResourceType::None;
    if (const auto type = json.find("type"); type != json.end())
    {
        if (const auto parse = magic_enum::enum_cast<ResourceMetadata::ResourceType>(type->get<std::string>()); parse.has_value())
            metadata.Type = parse.value();
    }

    if (const auto require = json.find("require"); require != json.end() && !require->empty())
    {
        for (auto [key, resource]: require->items())
            metadata.Require[key] = resource;
    }

    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (const auto objects = attributes->find("objects"); objects != attributes->end())
        {
            for (auto [key, object]: objects->items())
                metadata.Objects.push_back(ObjectMetadata{key, object});
        }
    }

    return true;
}
