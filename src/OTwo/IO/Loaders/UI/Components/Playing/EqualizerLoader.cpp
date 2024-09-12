#include <OTwo/IO/Loaders/UI/Components/Playing/EqualizerLoader.hpp>
#include <OTwo/Metadata/UI/Components/Playing/EqualizerMetadata.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Equalizer> EqualizerLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    auto metadata = EqualizerMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, context);

    if (auto count = attributes.find("count"); count != attributes.end())
        metadata.Count = count->get<int>();

    if (auto spacing = attributes.find("spacing"); spacing != attributes.end())
        metadata.Spacing = spacing->get<float>();

    if (auto it = metadata.Require.find("gauge"); it != metadata.Require.end())
    {
        auto prefab = std::any_cast<Gx::Json>(it->second);
        if (auto data = prefab.find("name"); data != prefab.end())
            metadata.ItemName = data->get<std::string>();

        if (auto data = prefab.find("source"); data != prefab.end())
            metadata.ItemSource = data.value();
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Equalizer> EqualizerLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const EqualizerMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    auto equalizer = std::make_unique<Equalizer>(metadata->Count, metadata->Spacing);

    equalizer->SetName(metadata->Name);
    equalizer->SetOrigin(metadata->Origin);
    equalizer->SetPosition(metadata->Position);
    equalizer->SetScale(metadata->Scale);
    equalizer->SetRotation(metadata->Rotation);

    if (context.Available())
    {
        auto container = ObjectContainer::Decorate(equalizer.get());
        for (int i = 0; i < metadata->Count; i++)
        {
            auto name = meta.Name + "/" + metadata->ItemName + std::to_string(i + 1);
            auto ctx  = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, metadata->ItemSource, container, ctx);
        }
    }

    return equalizer;
}
