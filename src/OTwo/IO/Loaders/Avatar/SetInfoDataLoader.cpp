#include <OTwo/IO/Loaders/Avatar/SetInfoDataLoader.hpp>
#include <OTwo/IO/AdlSerializer.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Serializable.g.hpp>

Gx::ResourcePtr<SetInfoData> SetInfoDataLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    if (json.empty())
        return nullptr;

    auto metadata = SetInfoData();
    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (const auto version = attributes->find("version"); version != attributes->end())
            metadata.Attributes = { version->get<std::string>() };
    }

    metadata.Require = SetInfoDependency{std::map<std::uint32_t, SetInfoItem>()};
    for (const auto [key, data] : json.at("require").at("sets").items())
    {
        if (data.empty())
            continue;

        auto setInfo = SetInfoItem();
        setInfo.Require = { data.at("require").at("items").get<std::vector<std::uint32_t>>() };
        if (setInfo.Require->Items->empty())
            continue;

        const auto attributes = data.at("attributes");
        setInfo.Attributes = {
            attributes.at("id").get<std::uint32_t>(),
            attributes.at("name").get<std::string>(),
            attributes.at("description").get<std::string>(),
            attributes.at("gender").get<Gender>(),
            attributes.at("origin").get<Planet>(),
            attributes.at("discounts").get<std::map<std::uint32_t, std::map<Currency, std::uint32_t>>>()
        };

        if (setInfo.Attributes->ID != std::stoi(key))
            throw Gx::ResourceLoadException("Inconsistent Set Info ID: " + key);

        metadata.Require->Sets->insert_or_assign(std::stoi(key), setInfo);
    }

    return std::make_unique<SetInfoData>(metadata);
}

Gx::ResourcePtr<SetInfoData> SetInfoDataLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    throw Gx::NotSupportedException();
}
