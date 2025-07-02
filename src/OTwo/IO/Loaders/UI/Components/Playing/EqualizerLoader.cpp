#include <OTwo/IO/Loaders/UI/Components/Playing/EqualizerLoader.hpp>
#include <OTwo/Metadata/UI/Components/Playing/EqualizerMetadata.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Equalizer> EqualizerLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    auto metadata = EqualizerMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (const auto attributes = json.find("attributes"); attributes != json.end())
    {
        if (const auto transform = attributes->find("transform"); transform != attributes->end())
            TransformLoader::ParseMetadata(transform.value(), metadata, context);

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

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Equalizer> EqualizerLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const EqualizerMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto equalizer  = std::make_unique<Equalizer>();
    auto container  = ObjectContainer::Decorate(equalizer.get());
    auto ctx        = ResourceContextDecorator::Decorate(context);
    equalizer->SetName(metadata->Name);
    equalizer->SetOrigin(metadata->Origin);
    equalizer->SetPosition(metadata->Position);
    equalizer->SetScale(metadata->Scale);
    equalizer->SetRotation(metadata->Rotation);
    equalizer->SetLocalBounds(metadata->Bounds);

    auto metaLoader = MetadataLoader();
    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        auto name = fmt::format("{}/{}", meta.Name, key);
        ObjectLoader::LoadFromJson(name, reference, container, ctx);
    }

    LoadChildren(container, meta, context);

    return equalizer;
}
