#ifndef O2JAM_MAP_SELECTOR_LOADER_HPP
#define O2JAM_MAP_SELECTOR_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/Components/Waiting/MapSelector.hpp>

class MapSelectorLoader : public ResourceLoader<MapSelector>
{
public :
    MapSelectorLoader() = default;

    Gx::ResourcePtr<MapSelector> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<MapSelector> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
