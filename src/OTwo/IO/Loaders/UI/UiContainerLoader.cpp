#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/UI/UiContainerMetadata.hpp>
#include <OTwo/UI/Room/UserList.hpp>

#include <OTwo/StringTable/Identifiers/Room.hpp>
using namespace StringTable::Identifiers;

Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
{
    UiContainerMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (const auto transform = attributes->find("transform"); transform != attributes->end())
            TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

        metadata.Bounds = {};
        if (const auto bounds = attributes->find("bounds"); bounds != attributes->end())
        {
            metadata.Bounds = {
                {},
                {
                    bounds->at("width"),
                    bounds->at("height")
                }
            };
        }
    }

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    auto metadata = dynamic_cast<const UiContainerMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto container = Instantiate(context);
    if (context.GetID() == Resource::Room::IDC_USER_LIST)
        container = std::make_unique<UserList>();

    auto populator = ObjectContainer::Decorate(container.get());
    auto ctx       = ResourceContextDecorator::Decorate(context);
    container->SetName(metadata->Name);
    container->SetOrigin(metadata->Origin);
    container->SetPosition(metadata->Position);
    container->SetScale(metadata->Scale);
    container->SetRotation(metadata->Rotation);
    container->SetLocalBounds(metadata->Bounds);

    auto metaLoader = MetadataLoader();
    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        auto name = fmt::format("{}/{}", meta.Name, key);
        ObjectLoader::Load(name, reference, populator, ctx);
    }

    LoadChildren(populator, meta, context);

    return container;
}

