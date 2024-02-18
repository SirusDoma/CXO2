#include <OTwo/Loaders/Character/AvatarLoader.hpp>

#include <OTwo/Metadata/Character/AvatarMetadata.hpp>
#include <OTwo/Loaders/TransformLoader.hpp>

AvatarLoader::AvatarLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> AvatarLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    AvatarMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json["attributes"];
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    return std::make_unique<AvatarMetadata>(metadata);
}

Gx::ResourcePtr<Avatar> AvatarLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const AvatarMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto avatar = std::make_unique<Avatar>();
    avatar->SetName(context.Name);
    avatar->SetOrigin(spec->Origin);
    avatar->SetPosition(spec->Position);
    avatar->SetScale(spec->Scale);
    avatar->SetRotation(spec->Rotation);

    return avatar;
}
