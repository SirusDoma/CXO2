#include <O2/Loaders/Character/ItemLoader.hpp>

#include <Genode/Utilities/StringHelper.hpp>
#include <magic_enum.hpp>

ItemLoader::ItemLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ItemLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ItemMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    auto attributes = json["attributes"];
    if (!attributes.empty())
    {
        metadata.ID          = attributes["id"].get<unsigned int>();
        metadata.Name        = attributes["name"].get<std::string>();
        metadata.Description = attributes["description"].get<std::string>();
        metadata.IsNew       = attributes["isNew"].get<bool>();

        if (auto parse = magic_enum::enum_cast<Character::Gender>( attributes["gender"].get<std::string>()); parse.has_value())
            metadata.Gender = parse.value();

        if (auto parse = magic_enum::enum_cast<Planet::Planet>(attributes["planet"].get<std::string>()); parse.has_value())
            metadata.Planet = parse.value();

        if (auto parse = magic_enum::enum_cast<Equipment::Type>(attributes["type"].get<std::string>()); parse.has_value())
            metadata.EquipmentType = parse.value();

        auto price =  attributes["price"];
        if (!price.empty())
        {
            if (auto parse = magic_enum::enum_cast<Shop::Currency>(price["currency"].get<std::string>()); parse.has_value())
                metadata.PriceCurrency = parse.value();

            if (metadata.PriceCurrency == Shop::Currency::Gem)
                metadata.Price = price["gem"].get<unsigned int>();
            else
                metadata.Price = price["mCash"].get<unsigned int>();
        }

    }

    auto require = json["require"];
    if (!require.empty())
    {
        auto preview = require["preview"];
        if (!preview.empty())
        {
            metadata.SmallPreview = preview["small"].get<std::string>();
            metadata.LargePreview = preview["large"].get<std::string>();
        }

        auto gender     = Character::Gender::Male;
        auto renderType = Equipment::RenderType::Body;
        auto instrument = Equipment::Instrument::None;

        for (auto [genderKey, partAttributes] : require.items())
        {
            auto key = Gx::StringHelper::ToPascalCase(genderKey);
            if (auto parse = magic_enum::enum_cast<Character::Gender>( std::string(key)); parse.has_value())
                gender = parse.value();
            else
                continue;

            for (auto [partKey, instrumentAttributes] : partAttributes.items())
            {
                key = Gx::StringHelper::ToPascalCase(partKey);
                if (auto parse = magic_enum::enum_cast<Equipment::RenderType>( std::string(key)); parse.has_value())
                    renderType = parse.value();
                else
                    continue;

                for (auto [instrumentKey, reference] : instrumentAttributes.items())
                {
                    key = Gx::StringHelper::ToPascalCase(instrumentKey);
                    // Hack: transform "Default" to "None", which one should we preserve?
                    if (key == "Default")
                        key = "None";

                    if (auto parse = magic_enum::enum_cast<Equipment::Instrument>( std::string(key)); parse.has_value())
                        instrument = parse.value();
                    else
                        continue;

                    metadata.References.push_back(ItemMetadata::ItemSpriteMetadata{gender, renderType, instrument, reference.get<std::string>()});
                }
            }
        }
    }

    return std::make_unique<ItemMetadata>(metadata);
}

Gx::ResourcePtr<Item> ItemLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ItemMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto item = std::make_unique<Item>();
    item->SetId(spec->ID);
    item->SetName(spec->Name);
    item->SetDescription(spec->Description);
    item->SetIsNew(spec->IsNew);
    item->SetType(spec->EquipmentType);
    item->SetGender(spec->Gender);
    item->SetPlanet(spec->Planet);
    item->SetPriceCurrency(spec->PriceCurrency);
    item->SetPrice(spec->Price);

    if (!spec->SmallPreview.isEmpty())
        item->SetSmallPreview(context.Resources->Resolve<Gx::Sprite>(spec->SmallPreview));

    if (!spec->LargePreview.isEmpty())
        item->SetLargePreview(context.Resources->Resolve<Gx::Sprite>(spec->LargePreview));

    for (auto ref : spec->References)
        item->SetRenderableItem(ref.Gender, ref.RenderType, ref.Instrument, context.Resources->Resolve<Gx::Animation>(ref.Reference));

    return item;
}
