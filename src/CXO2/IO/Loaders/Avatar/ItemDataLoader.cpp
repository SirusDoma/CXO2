#include <CXO2/O2.hpp>
#include <CXO2/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <CXO2/IO/Loaders/Avatar/ItemLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Utilities/StringTranscoder.hpp>

#include <magic_enum/magic_enum.hpp>
#include <mutex>

namespace Cx
{
    namespace
    {
        template<typename T>
        void Read(sf::InputStream& stream, const Gx::ResourceContext& ctx, T& value)
        {
            if (stream.read(&value, sizeof(value)) != sizeof(value))
                throw Gx::ResourceLoadException(ctx.GetID(), "Failed to load ItemData file");
        }

        sf::String ReadString(sf::InputStream& stream, const Gx::ResourceContext& ctx)
        {
            std::uint32_t length;
            Read(stream, ctx, length);

            if (length == 0)
                return sf::String();

            auto bytes = std::vector<char>(length);
            if (stream.read(bytes.data(), length) != length)
                throw Gx::ResourceLoadException(ctx.GetID(), "Failed to load ItemData file");

            return StringTranscoder::Transcode(bytes.data(), bytes.size());
        }

        Gx::Json CreateRef(sf::InputStream& stream, const Gx::ResourceContext& ctx, const std::string& type)
        {
            bool valid;
            Read(stream, ctx, valid);

            if (!valid)
                return Gx::Json::object();

            std::uint32_t refLength;
            Read(stream, ctx, refLength);

            if (refLength == 0)
                return Gx::Json::object();

            auto refBytes = std::vector<char>(refLength);
            if (stream.read(refBytes.data(), refLength) != refLength)
                throw Gx::ResourceLoadException(ctx.GetID(), "Failed to load ItemData file");

            auto ref = std::string(refBytes.begin(), refBytes.end());
            if (!Gx::FileSystem::Contains(ref))
                ref = fmt::format("{}.ojs", Gx::StringHelper::RemoveExtension(ref));

            auto json = Gx::Json();
            json["type"]    = type;
            json["require"] = { { "sheet", ref } };

            if (type == Gx::StringHelper::GetTypeName<Gx::Animation>(false))
                json["attributes"] = { { "inherit", true } };

            return json;
        }
    }

    Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        if (Gx::StringHelper::EndsWith(fileName.string(),".json"))
            return ResourceLoader::LoadFromFile(fileName, ctx);

        const auto inputStream = Gx::FileSystem::Open(fileName);
        if (!inputStream)
            throw Gx::ResourceLoadException(fileName.string());

        auto& stream = *inputStream.get();

        auto itemData = Instantiate(ctx);
        std::uint32_t count;
        Read(stream, ctx, count);

        #pragma pack(push, 1)
        struct ItemInfo
        {
            std::uint32_t ID;
            EquipmentType Type;
            Cx::Planet    Planet;
            std::uint16_t Flag;
            std::uint8_t  Quantity; // newer version: std::uint16_t
            std::uint8_t  MoidifierEffect;
            std::uint8_t  MoidifierCategory;
            Cx::Currency  Currency;
            std::uint32_t PriceGem;
            std::uint32_t PriceCash;
        };
        #pragma pack(pop)

        for (std::size_t i = 0; i < count; i++)
        {
            ItemInfo info;
            Read(stream, ctx, info);

            std::uint8_t slot;
            Read(stream, ctx, slot);

            auto item = ItemMetadata();
            item.ID            = info.ID;
            item.EquipmentType = info.Type;
            item.Origin        = info.Planet;
            item.IsNew         = info.Flag >> 11 & 1;

            switch (info.Flag >> 7 & 15)
            {
                case 0:  item.Gender = Gender::Female; break;
                case 1:  item.Gender = Gender::Male;   break;
                default: item.Gender = Gender::Any;    break;
            }

            item.Name        = ReadString(stream, ctx);
            item.Description = ReadString(stream, ctx);

            item.Prices = {};
            if (info.Currency != Currency::None)
                item.Prices[info.Currency] = info.Currency == Currency::Cash ? info.PriceCash : info.PriceGem;

            for (const auto [renderPart, _] : magic_enum::enum_entries<RenderPart>())
            {
                if (renderPart == RenderPart::SmallThumbnail || renderPart == RenderPart::LargeThumbnail)
                {
                    if (renderPart == RenderPart::SmallThumbnail)
                        item.SmallThumbnail = CreateRef(stream, ctx, Gx::StringHelper::GetTypeName<Gx::Sprite>(false));
                    else
                        item.LargeThumbnail = CreateRef(stream, ctx, Gx::StringHelper::GetTypeName<Gx::Sprite>(false));

                    continue;
                }

                for (const auto [instrument, _] : magic_enum::enum_entries<Instrument>())
                {
                    for (const auto gender : { Gender::Male, Gender::Female })
                    {
                        auto ref = CreateRef(stream, ctx, Gx::StringHelper::GetTypeName<Gx::Animation>(false));
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

        itemData->Version = O2::Version;
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
        const auto items = metadata.Require.find("items");
        if (items == metadata.Require.end())
            return nullptr;

        for (auto [key, resource] : items->second.items())
        {
            unsigned int id = std::stoi(key);
            if (auto itemMetadata = ItemMetadata(); ItemLoader::ParseMetadata(resource, itemMetadata, context))
                metadata.Items[id] = itemMetadata;
        }

        return std::make_unique<ItemData>(metadata);
    }

    Gx::ResourcePtr<ItemData> ItemDataLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ItemData*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException(context.GetID(), "The specified metadata is incompatible");

        return std::make_unique<ItemData>(*metadata);
    }
}
