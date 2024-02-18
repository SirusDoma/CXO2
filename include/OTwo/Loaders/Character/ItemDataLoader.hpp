#ifndef O2JAM_ITEM_DATA_LOADER_HPP
#define O2JAM_ITEM_DATA_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/Metadata/Character/ItemData.hpp>

class ItemDataLoader : public O2JamResourceLoader<ItemData>
{
public :
    ItemDataLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<ItemData> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif