#include <OTwo/IO/Loaders/UI/Components/Room/UserListLoader.hpp>
#include <OTwo/Metadata/UI/Components/Room/UserListMetadata.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <Genode/UI/List.hpp>


Gx::ResourcePtr<UserList> UserListLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    UserListMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (const auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<UserList> UserListLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const UserListMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto userList  = std::make_unique<UserList>();
    auto container = ObjectContainer::Decorate(userList.get());
    auto ctx       = ResourceContextDecorator::Decorate(context);
    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, object, container, ctx);
    }

    userList->SetName(meta.Name);
    userList->SetOrigin(metadata->Origin);
    userList->SetPosition(metadata->Position);
    userList->SetScale(metadata->Scale);
    userList->SetRotation(metadata->Rotation);

    return userList;
}
