#include <CXO2/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Metadata/UI/Components/Waiting/AvatarInfoMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<AvatarInfo> AvatarInfoLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        AvatarInfoMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (auto transform = attributes.find("transform"); transform != attributes.end())
                TransformLoader::ParseMetadata(transform.value(), metadata, context);

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
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<AvatarInfo> AvatarInfoLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const AvatarInfoMetadata*>(&meta);
        if (metadata == nullptr)
            return Instantiate(context);

        auto avatarinfo = Instantiate(context);
        auto container  = SceneComposer::Compose(*avatarinfo);
        auto ctx        = ResourceContextDecorator::Decorate(context);
        avatarinfo->SetName(metadata->Name);

        if (metadata->Position.has_value())
        {
            avatarinfo->SetPosition(*metadata->Position);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            avatarinfo->SetPosition({
                static_cast<float>(bound->position.x),
                static_cast<float>(bound->position.y),
            });
        }

        for (auto& [team, color] : metadata->TeamColors)
            avatarinfo->RegisterTeamColor(team, color);

        LoadChildren(container, meta, context);

        avatarinfo->SetOrigin(metadata->Origin);
        avatarinfo->SetScale(metadata->Scale);
        avatarinfo->SetRotation(metadata->Rotation);

        return avatarinfo;
    }
}
