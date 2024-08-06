#include <OTwo/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Avatar/Item.hpp>

Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    if (json.empty())
        return nullptr;

    auto metadata = ItemData();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (auto version = attributes->find("version"); version != attributes->end())
            metadata.Version = version->get<std::string>();
    }

    metadata.Items = std::map<unsigned int, ItemMetadata>();
    auto items = std::any_cast<Gx::Json>(metadata.Require["items"]);
    for (auto [key, resource] : items.items())
    {
        unsigned int id = std::stoi(key);
        auto data = std::any_cast<Gx::Json>(resource);
        if (auto itemMetadata = ItemMetadata(); ItemLoader::ParseMetadata(data, itemMetadata, context))
            metadata.Items[id] = itemMetadata;
    }

    return std::make_unique<ItemData>(metadata);
}

Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ItemData*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");

    return std::make_unique<ItemData>(*metadata);
}
