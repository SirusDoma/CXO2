#include <OTwo/Loaders/Character/ItemDataLoader.hpp>
#include <OTwo/Character/Item.hpp>

ItemDataLoader::ItemDataLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ItemDataLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ItemData metadata;

    if (auto type =  json["type"]; !type.empty())
        metadata.ResourceType = type.get<std::string>();

    auto attributes = json["attributes"];
    if (!attributes.empty())
    {
        auto version = attributes["version"];
        if (!version.empty())
            metadata.Version = version.get<std::string>();
    }

    metadata.Items = std::map<unsigned int, ItemMetadata>();
    auto require = json["require"];
    auto loader  = Gx::ResourceLoaderFactory::GetLoader<Item>();
    if (loader && !require.empty())
    {
        auto items = require["items"];
        if (!items.empty())
        {
            for (auto [idData, itemData] : items.items())
            {
                unsigned int id  = std::stoi(idData);
                std::string item = itemData.dump();

                auto itemMeta = loader->LoadMetadata(reinterpret_cast<void*>(&item[0]), item.size());
                metadata.Items[id] = *(static_cast<ItemMetadata*>(itemMeta.get())); // converse to ItemMetadata first to prevent slicing
            }
        }
    }

    return std::make_unique<ItemData>(metadata);
}

Gx::ResourcePtr<ItemData> ItemDataLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    return nullptr;
}
