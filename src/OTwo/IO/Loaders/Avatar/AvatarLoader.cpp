#include <OTwo/IO/Loaders/Avatar/AvatarLoader.hpp>

#include <OTwo/Metadata/Avatar/AvatarMetadata.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    AvatarMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;
    
    auto attributes = json.at("attributes");

    if (auto transform = attributes.find("transform"); transform != attributes.end())
    {
        if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
            return nullptr;
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Avatar> AvatarLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto avatar = std::make_unique<Avatar>();
    auto metadata = dynamic_cast<const AvatarMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    avatar->SetName(metadata->Name);
    avatar->SetOrigin(metadata->Origin);
    avatar->SetPosition(metadata->Position);
    avatar->SetScale(metadata->Scale);
    avatar->SetRotation(metadata->Rotation);

    return avatar;
}
