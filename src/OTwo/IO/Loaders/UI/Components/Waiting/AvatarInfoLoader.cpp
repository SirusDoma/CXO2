#include <OTwo/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/UI/Components/Waiting/AvatarInfoMetadata.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<AvatarInfo> AvatarInfoLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    AvatarInfoMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    if (auto teamColors = attributes.find("teamColors"); teamColors != attributes.end())
    {
        for (auto& [key, color] : teamColors->items())
        {
            if (auto team = magic_enum::enum_cast<RoomTeam>(key, magic_enum::case_insensitive); team.has_value() && !color.empty())
            {
                unsigned int a, r, g, b;
                color.at("a").get_to(a);
                color.at("r").get_to(r);
                color.at("g").get_to(g);
                color.at("b").get_to(b);
                metadata.TeamColors[team.value()] = sf::Color(r, g, b, a);
            }
        }
    }

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<AvatarInfo> AvatarInfoLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const AvatarInfoMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto avatarinfo = std::make_unique<AvatarInfo>();
    auto container  = ObjectContainer::Decorate(avatarinfo.get());
    auto ctx        = ResourceContextDecorator::Decorate(context);
    avatarinfo->SetName(metadata->Name);

    for (auto& [team, color] : metadata->TeamColors)
        avatarinfo->RegisterTeamColor(team, color);

    auto metaLoader = MetadataLoader();
    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, reference, container, ctx);
    }

    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, object, container, ctx);
    }

    avatarinfo->SetOrigin(metadata->Origin);
    avatarinfo->SetPosition(metadata->Position);
    avatarinfo->SetScale(metadata->Scale);
    avatarinfo->SetRotation(metadata->Rotation);

    return avatarinfo;
}
