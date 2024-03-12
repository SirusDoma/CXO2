#ifndef O2JAM_ITEM_DATA_LOADER_HPP
#define O2JAM_ITEM_DATA_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Avatar/ItemData.hpp>

class ItemDataLoader : public ResourceLoader<ItemData>
{
public :
    ItemDataLoader() = default;

    Gx::ResourcePtr<ItemData> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<ItemData> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif