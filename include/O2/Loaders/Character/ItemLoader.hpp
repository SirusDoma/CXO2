#ifndef O2JAM_CHARACTER_ITEM_LOADER_HPP
#define O2JAM_CHARACTER_ITEM_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>

#include <O2/Metadata/Character/ItemMetadata.hpp>
#include <O2/Character/Item.hpp>

class ItemLoader : public O2JamResourceLoader<Item>
{
public :
    ItemLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Item> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
