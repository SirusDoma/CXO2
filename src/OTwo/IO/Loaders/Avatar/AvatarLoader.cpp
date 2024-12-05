#include <OTwo/IO/Loaders/Avatar/AvatarLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <OTwo/Metadata/Avatar/AvatarMetadata.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/System/Application.hpp>

#include <magic_enum.hpp>

AvatarLoader::AvatarLoader(ItemFactory& items) :
    m_items(&items)
{
}

Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    AvatarMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;
    
    auto attributes = json.at("attributes");
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

    if (const auto transform = attributes.find("transform"); transform != attributes.end())
    {
        if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
            return nullptr;
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    auto avatar = std::make_unique<Avatar>();
    const auto metadata = dynamic_cast<const AvatarMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    avatar->SetName(metadata->Name);
    avatar->SetGender(metadata->Gender);
    avatar->SetOrigin(metadata->Origin);
    avatar->SetPosition(metadata->Position);
    avatar->SetScale(metadata->Scale);
    avatar->SetRotation(metadata->Rotation);

    for (auto& [_, item] : m_items->GetDefaultItems(avatar->GetGender()))
        avatar->Equip(std::move(item));

    for (const auto id : metadata->ItemIDs)
        avatar->Equip(m_items->Create(id));

    auto container = ObjectContainer::Decorate(avatar.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(context, name);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    return avatar;
}
