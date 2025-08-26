#include <OTwo/O2Jam.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Utilities/StringTranscoder.hpp>

#include <magic_enum/magic_enum.hpp>
#include <mutex>

Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
{
    if (Gx::StringHelper::EndsWith(fileName,".json"))
        return ResourceLoader::LoadFromFile(fileName, ctx);

    const auto inputStream = Gx::FileSystem::Open(fileName);
    if (!inputStream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& stream = *inputStream.get();

    auto itemData = Instantiate(ctx);
    std::uint32_t count;
    if (stream.read(&count, sizeof(count)) != sizeof(uint32_t))
        throw Gx::ResourceLoadException("Failed to load ItemData file");

    struct ItemInfo
    {
        std::uint32_t ID;
        EquipmentType Type;
        ::Planet      Planet;
        std::uint16_t Flag;
        std::uint8_t  Quantity; // newer version: std::uint16_t
        std::uint8_t  MoidifierEffect;
        std::uint8_t  MoidifierCategory;
        ::Currency    Currency;
        std::uint32_t PriceGem;
        std::uint32_t PriceCash;
    };

    const auto createRef = [&] (const std::string& type, sf::InputStream& input)
    {
        bool valid;
        if (input.read(&valid, sizeof(valid)) != sizeof(bool))
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        if (!valid)
            return Gx::Json::object();

        std::uint32_t refLength;
        if (input.read(&refLength, sizeof(refLength)) != sizeof(uint32_t))
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        if (refLength == 0)
            return Gx::Json::object();

        auto refBytes = std::vector<char>(refLength);
        if (input.read(refBytes.data(), refLength) != refLength)
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        auto ref = std::string(refBytes.begin(), refBytes.end());
        if (!Gx::FileSystem::Contains(ref))
            ref = fmt::format("{}.ojs", Gx::StringHelper::RemoveExtension(ref));

        auto json = Gx::Json();
        json["type"]       = type;
        json["require"]    = { { "sheet", ref } };

        if (type == Gx::StringHelper::GetTypeName<Gx::Animation>(false))
            json["attributes"] = { { "inherit", true } };

        return json;
    };

    for (std::size_t i = 0; i < count; i++)
    {
        ItemInfo info;
        if (stream.read(&info, sizeof(ItemInfo)) != sizeof(ItemInfo))
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        std::uint8_t part;
        if (stream.read(&part, 1) != 1)
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        std::uint32_t fnLength;
        if (stream.read(&fnLength, sizeof(fnLength)) != sizeof(uint32_t))
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        auto fn = std::vector<char>(fnLength);
        if (stream.read(fn.data(), fnLength) != fnLength)
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        std::uint32_t dsLength;
        if (stream.read(&dsLength, sizeof(dsLength)) != sizeof(uint32_t))
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        auto ds = std::vector<char>(dsLength);
        if (stream.read(ds.data(), dsLength) != dsLength)
            throw Gx::ResourceLoadException("Failed to load ItemData file");

        auto item = ItemMetadata();
        item.ID            = info.ID;
        item.EquipmentType = info.Type;
        item.Origin        = info.Planet;
        item.IsNew         = (info.Flag >> 11) & 1;

        switch ((info.Flag >> 7) & 15)
        {
            case 0:  item.Gender = Gender::Female; break;
            case 1:  item.Gender = Gender::Male;   break;
            default: item.Gender = Gender::Any;    break;
        }

        item.Name          = sf::String::fromUtf8(fn.begin(), fn.end());
        item.Description   = sf::String::fromUtf8(ds.begin(), ds.end());

        item.Prices = {};
        if (info.Currency != Currency::None)
            item.Prices[info.Currency] = info.Currency == Currency::Cash ? info.PriceCash : info.PriceGem;

        for (const auto [renderPart, _] : magic_enum::enum_entries<RenderPart>())
        {
            if (renderPart == RenderPart::SmallThumbnail || renderPart == RenderPart::LargeThumbnail)
            {
                if (renderPart == RenderPart::SmallThumbnail)
                    item.SmallThumbnail = createRef(Gx::StringHelper::GetTypeName<Gx::Sprite>(false), stream);
                else
                    item.LargeThumbnail = createRef(Gx::StringHelper::GetTypeName<Gx::Sprite>(false), stream);

                continue;
            }

            for (const auto [instrument, _] : magic_enum::enum_entries<Instrument>())
            {
                for (const auto gender : { Gender::Male, Gender::Female })
                {
                    auto ref = createRef(Gx::StringHelper::GetTypeName<Gx::Animation>(false), stream);
                    if (ref.empty())
                        continue;

                    item.References.push_back({
                        gender,
                        renderPart,
                        instrument,
                        ref
                    });
                }
            }
        }

        if (item.ID == 0)
            continue;

        itemData->Items[item.ID] = item;
    }

    itemData->Version = O2Jam::Version;
    return itemData;
}

Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    if (json.empty())
        return nullptr;

    auto metadata = ItemData();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (const auto version = attributes->find("version"); version != attributes->end())
            metadata.Version = version->get<std::string>();
    }

    metadata.Items = std::map<unsigned int, ItemMetadata>();
    auto items = std::any_cast<Gx::Json>(metadata.Require["items"]);
    for (auto [key, resource] : items.items())
    {
        unsigned int id = std::stoi(key);
        const auto data = std::any_cast<Gx::Json>(resource);
        if (auto itemMetadata = ItemMetadata(); ItemLoader::ParseMetadata(data, itemMetadata, context))
            metadata.Items[id] = itemMetadata;
    }

    return std::make_unique<ItemData>(metadata);
}

Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ItemData*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    return std::make_unique<ItemData>(*metadata);
}
