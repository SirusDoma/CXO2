#include <CXO2/O2.hpp>
#include <CXO2/IO/Loaders/Avatar/SetInfoDataLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/AdlSerializer.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/Avatar/SetInfoData.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/Utilities/StringTranscoder.hpp>

#include <algorithm>

namespace Cx
{
    namespace
    {
        template<typename T>
        void Read(sf::InputStream& stream, const Gx::ResourceContext& ctx, T& value)
        {
            if (stream.read(&value, sizeof(value)) != sizeof(value))
                throw Gx::ResourceLoadException(ctx.GetID(), "Failed to load SetInfoData file");
        }

        sf::String ReadString(sf::InputStream& stream, const Gx::ResourceContext& ctx)
        {
            std::uint32_t length;
            Read(stream, ctx, length);

            if (length == 0)
                return sf::String();

            auto bytes = std::vector<char>(length);
            if (stream.read(bytes.data(), length) != length)
                throw Gx::ResourceLoadException(ctx.GetID(), "Failed to load SetInfoData file");

            return StringTranscoder::Transcode(bytes.data(), bytes.size());
        }
    }

    Gx::ResourcePtr<SetInfoData> SetInfoDataLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        if (Gx::StringHelper::EndsWith(fileName.string(), ".json"))
            return ResourceLoader::LoadFromFile(fileName, ctx);

        const auto inputStream = Gx::FileSystem::Open(fileName);
        if (!inputStream)
            throw Gx::ResourceLoadException(fileName.string());

        auto& stream = *inputStream.get();

        auto setInfoData = Instantiate(ctx);
        std::uint32_t count;
        Read(stream, ctx, count);

        #pragma pack(push, 1)
        struct SetInfo
        {
            std::uint32_t ID;
            std::uint8_t  OriginFlag;
            std::uint8_t  GenderFlag;
            std::uint8_t  Discounted;
            std::uint8_t  ItemCount;
            std::uint8_t  Currency;
            std::uint32_t ItemIDs[5];
            std::uint32_t Prices[5];
            std::uint32_t Discounts[5];
        };
        #pragma pack(pop)

        for (std::size_t i = 0; i < count; i++)
        {
            SetInfo info;
            Read(stream, ctx, info);

            auto set   = SetInfoMetadata();
            set.ID     = info.ID;
            set.Type   = ResourceMetadata::ResourceType::SetInfo;
            set.IsNew  = (info.OriginFlag >> 7 & 1) != 0;
            set.Origin = static_cast<Planet>(info.OriginFlag & 0x7F);

            switch (info.GenderFlag >> 6)
            {
                case 0:  set.Gender = Gender::Female; break;
                case 1:  set.Gender = Gender::Male;   break;
                default: set.Gender = Gender::Any;    break;
            }

            set.Name        = ReadString(stream, ctx);
            set.Description = ReadString(stream, ctx);

            const auto currency = info.Currency != 0 ? Currency::Gem : Currency::Cash;
            for (std::size_t slot = 0; slot < std::min<std::size_t>(info.ItemCount, 5); slot++)
            {
                set.ItemsIDs.insert(info.ItemIDs[slot]);
                if (info.Discounted != 0 && info.Prices[slot] > info.Discounts[slot])
                    set.Discounts[info.ItemIDs[slot]][currency] = info.Prices[slot] - info.Discounts[slot];
            }

            if (set.ID == 0 || set.ItemsIDs.empty())
                continue;

            setInfoData->Sets[set.ID] = set;
        }

        setInfoData->Version = O2::Version;
        return setInfoData;
    }

    Gx::ResourcePtr<SetInfoData> SetInfoDataLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        if (json.empty())
            return nullptr;

        auto metadata = SetInfoData();
        if (!MetadataLoader::Parse(json, metadata, context))
            return nullptr;

        if (auto attributes = json.find("attributes"); attributes != json.end())
        {
            if (auto version = attributes->find("version"); version != attributes->end())
                metadata.Version = version->get<std::string>();
        }

        metadata.Sets = std::map<unsigned int, SetInfoMetadata>();
        const auto sets = metadata.Require.find("sets");
        if (sets == metadata.Require.end())
            return nullptr;

        for (auto [key, resource] : sets->second.items())
        {
            unsigned int id = std::stoi(key);
            auto setInfoData = resource;

            auto setInfoMetadata = SetInfoMetadata();
            if (!MetadataLoader::Parse(setInfoData, setInfoMetadata, context))
                continue;

            const auto& require = setInfoMetadata.Require;
            if (require.empty())
                continue;

            if (const auto it = require.find("items"); it != require.end())
            {
                for (const auto& itemID : it->second.items())
                    setInfoMetadata.ItemsIDs.insert(itemID.value().get<unsigned int>());
            }

            if (setInfoMetadata.ItemsIDs.empty())
                continue;


            if (auto attributes = setInfoData.find("attributes"); attributes != setInfoData.end())
            {
                setInfoMetadata.ID   = attributes->at("id").get<unsigned int>();
                setInfoMetadata.Type = ResourceMetadata::ResourceType::SetInfo;

                auto name                   = attributes->at("name").get<std::string>();
                auto description            = attributes->at("description").get<std::string>();
                setInfoMetadata.Name        = sf::String::fromUtf8(name.begin(), name.end());
                setInfoMetadata.Description = sf::String::fromUtf8(description.begin(), description.end());

                if (auto gender = magic_enum::enum_cast<Gender>(attributes->at("gender").get<std::string>()); gender.has_value())
                    setInfoMetadata.Gender = gender.value();

                if (auto origin = magic_enum::enum_cast<Planet>(attributes->at("origin").get<std::string>()); origin.has_value())
                    setInfoMetadata.Origin = origin.value();

                if (auto discounts = attributes->find("discounts"); discounts != attributes->end())
                    setInfoMetadata.Discounts = discounts.value().get<std::map<std::uint32_t, std::unordered_map<Currency, std::uint32_t>>>();
            }

            metadata.Sets[id] = setInfoMetadata;
        }

        return std::make_unique<SetInfoData>(metadata);
    }

    Gx::ResourcePtr<SetInfoData> SetInfoDataLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        throw Gx::NotSupportedException();
    }
}
