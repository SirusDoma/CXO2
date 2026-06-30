#include <CXO2/IO/Loaders/Avatar/SetInfoDataLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/AdlSerializer.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/Avatar/SetInfoData.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Planet.hpp>

namespace Cx
{
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
        auto sets = std::any_cast<Gx::Json>(metadata.Require["sets"]);
        for (auto [key, resource] : sets.items())
        {
            unsigned int id = std::stoi(key);
            auto setInfoData = std::any_cast<Gx::Json>(resource);

            auto setInfoMetadata = SetInfoMetadata();
            if (!MetadataLoader::Parse(setInfoData, setInfoMetadata, context))
                continue;

            const auto& require = setInfoMetadata.Require;
            if (require.empty())
                continue;

            if (const auto it = require.find("items"); it != require.end() && it->second.type() == typeid(Gx::Json))
            {
                auto table = std::any_cast<Gx::Json>(it->second);
                for (const auto& itemID : table.items())
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
