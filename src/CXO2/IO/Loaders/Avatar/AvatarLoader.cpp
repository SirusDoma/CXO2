#include <CXO2/IO/Loaders/Avatar/AvatarLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <CXO2/Metadata/Avatar/AvatarMetadata.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    AvatarLoader::AvatarLoader(ItemFactory& items) :
        m_items(&items)
    {
    }

    Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        AvatarMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (const auto items = attributes.find("items"); items != attributes.end())
            {
                for (const auto [_, id] : items->items())
                {
                    if (id.empty())
                        continue;

                    metadata.ItemIDs.push_back(id.get<unsigned int>());
                }
            }

            if (const auto gender = attributes.find("gender"); gender != attributes.end())
            {
                if (auto parsed = magic_enum::enum_cast<Gender>(gender->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                    metadata.Gender = parsed.value();
            }

            if (const auto offset = attributes.find("offset"); offset != attributes.end())
            {
                metadata.Offset = {
                    offset->at("x").get<float>(),
                    offset->at("y").get<float>()
                };
            }

            if (const auto transform = attributes.find("transform"); transform != attributes.end())
            {
                if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
                    return Instantiate(context);
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const AvatarMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto avatar = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (metadata->Position.has_value())
        {
            avatar->SetPosition(*metadata->Position);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            avatar->SetPosition({
               static_cast<float>(bound->position.x),
               static_cast<float>(bound->position.y)
           });
        }
    
        avatar->SetName(metadata->Name);
        avatar->SetGender(metadata->Gender);
        avatar->SetOrigin(metadata->Origin);
        avatar->SetOffset(metadata->Offset);
        avatar->SetScale(metadata->Scale);
        avatar->SetRotation(metadata->Rotation);

        for (auto& [_, item] : m_items->GetDefaultItems(avatar->GetGender()))
            avatar->Equip(std::move(item));

        for (const auto id : metadata->ItemIDs)
            avatar->Equip(m_items->Create(id));

        auto container = SceneComposer::Compose(*avatar);
        LoadChildren(container, meta, context);

        return avatar;
    }
}
