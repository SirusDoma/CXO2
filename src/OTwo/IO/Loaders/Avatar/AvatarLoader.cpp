#include <OTwo/IO/Loaders/Avatar/AvatarLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <OTwo/Metadata/Avatar/AvatarMetadata.hpp>


Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    AvatarMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;
    
    auto attributes = json.at("attributes");

    if (const auto transform = attributes.find("transform"); transform != attributes.end())
    {
        if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
            return nullptr;
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto avatar = std::make_unique<Avatar>();
    const auto metadata = dynamic_cast<const AvatarMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    avatar->SetName(metadata->Name);
    avatar->SetOrigin(metadata->Origin);
    avatar->SetPosition(metadata->Position);
    avatar->SetScale(metadata->Scale);
    avatar->SetRotation(metadata->Rotation);

    auto populator = ObjectPopulator::Decorate(avatar.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return avatar;
}
