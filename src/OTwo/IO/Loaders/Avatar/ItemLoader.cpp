#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <magic_enum.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>
#include <OTwo/Models/Shop.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Models/Character.hpp>

Gx::ResourcePtr<Item> ItemLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    if (json.empty())
        return nullptr;

    auto metadata = ItemMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (!ParseMetadata(json, metadata, context))
        return nullptr;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Item> ItemLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ItemMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto item = std::make_unique<Item>();
    const auto ctx = ResourceContextDecorator::Decorate(context);

    const auto spriteLoader = SpriteLoader();
    const auto animationLoader = AnimationLoader();
    item->SetID(metadata->ID);
    item->SetName(metadata->Name);
    item->SetDescription(metadata->Description);
    item->SetIsNew(metadata->IsNew);
    item->SetType(metadata->EquipmentType);
    item->SetGender(metadata->Gender);
    item->SetOrigin(metadata->Origin);

    for (auto [currency, price] : metadata->Prices)
        item->SetPrice(currency, price);

    if (!metadata->SmallPreview.isEmpty())
        item->SetSmallPreview(spriteLoader.LoadFromFile(metadata->SmallPreview, ctx));

    if (!metadata->LargePreview.isEmpty())
        item->SetLargePreview(spriteLoader.LoadFromFile(metadata->LargePreview, ctx));

    for (const auto& ref : metadata->References)
    {
        auto animation = animationLoader.LoadFromFile(ref.Reference, ctx);
        animation->SetLoop(true);

        item->SetRenderableItem(ref.Gender, ref.RenderPart, ref.Instrument, std::move(animation));
    }

    return item;
}

bool ItemLoader::ParseMetadata(Gx::Json json, ItemMetadata &metadata, const Gx::ResourceContext &context)
{
    if (json.empty())
        return false;

    MetadataLoader::Parse(json, metadata, context);
    if (auto attributes = json.find("attributes"); attributes != json.end())
    {
        metadata.ID          = attributes->at("id").get<unsigned int>();
        metadata.IsNew       = attributes->at("isNew").get<bool>();
        metadata.Type        = ResourceMetadata::ResourceType::Item;

        auto name            = attributes->at("name").get<std::string>();
        auto description     = attributes->at("description").get<std::string>();
        metadata.Name        = sf::String::fromUtf8(name.begin(), name.end());
        metadata.Description = sf::String::fromUtf8(description.begin(), description.end());

        if (auto gender = magic_enum::enum_cast<Gender>(attributes->at("gender").get<std::string>()); gender.has_value())
            metadata.Gender = gender.value();

        if (auto origin = magic_enum::enum_cast<Planet>(attributes->at("origin").get<std::string>()); origin.has_value())
            metadata.Origin = origin.value();

        if (auto equipType = magic_enum::enum_cast<EquipmentType>(attributes->at("type").get<std::string>()); equipType.has_value())
            metadata.EquipmentType = equipType.value();

        if (auto price = attributes->find("price"); price != attributes->end())
        {
            for (auto [priceKey, priceValue] : price->items())
            {
                std::string currencyString = Gx::StringHelper::ToPascalCase(priceKey);
                if (auto parse = magic_enum::enum_cast<Currency>(currencyString); parse.has_value())
                    metadata.Prices[parse.value()] = priceValue.get<unsigned int>();
            }
        }
    }

    if (!metadata.Require.empty())
    {
        if (auto it = metadata.Require.find("preview"); it != metadata.Require.end())
        {
            if (auto preview = std::any_cast<Gx::Json>(it->second); !preview.empty())
            {
                metadata.SmallPreview = preview.at("small").get<std::string>();
                metadata.LargePreview = preview.at("large").get<std::string>();
            }
        }

        for (auto [itemKey, partData] : metadata.Require)
        {
            auto gender     = Gender::Male;
            auto renderPart = RenderPart::Body;
            auto instrument = Instrument::None;

            if (auto parse = magic_enum::enum_cast<Gender>(std::string(Gx::StringHelper::ToPascalCase(itemKey))); parse.has_value())
                gender = parse.value();
            else
                continue;

            auto partAttributes = std::any_cast<Gx::Json>(partData);
            for (auto [partKey, instrumentAttributes] : partAttributes.items())
            {
                if (auto parse = magic_enum::enum_cast<RenderPart>(std::string(Gx::StringHelper::ToPascalCase(partKey))); parse.has_value())
                    renderPart = parse.value();
                else
                    continue;

                for (auto [instrumentKey, reference] : instrumentAttributes.items())
                {
                    auto key = Gx::StringHelper::ToPascalCase(instrumentKey);
                    // Hack: transform "Default" to "None", which one should we preserve?
                    if (key == "Default")
                        key = "None";

                    if (auto parse = magic_enum::enum_cast<Instrument>(std::string(key)); parse.has_value())
                        instrument = parse.value();
                    else
                        continue;

                    metadata.References.push_back(ItemMetadata::ItemSpriteMetadata{gender, renderPart, instrument, reference.get<std::string>()});
                }
            }
        }
    }

    return true;
}
