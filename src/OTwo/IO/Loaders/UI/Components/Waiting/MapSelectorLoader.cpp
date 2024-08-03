#include <OTwo/IO/Loaders/UI/Components/Waiting/MapSelectorLoader.hpp>
#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>

Gx::ResourcePtr<MapSelector> MapSelectorLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    const auto baseLoader = UiContainerLoader();
    if (const auto container = baseLoader.LoadFromJson(json, context); container)
        return std::make_unique<MapSelector>(std::move(*container));

    return nullptr;
}

Gx::ResourcePtr<MapSelector> MapSelectorLoader::LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    const auto baseLoader = UiContainerLoader();
    if (const auto container = baseLoader.LoadFromMetadata(metadata, context); container)
        return std::make_unique<MapSelector>(std::move(*container));

    return nullptr;
}

