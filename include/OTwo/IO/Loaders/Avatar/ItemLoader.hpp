#pragma once

#include <OTwo/IO/ResourceLoader.hpp>

#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>
#include <OTwo/Avatar/Item.hpp>

class ItemLoader : public ResourceLoader<Item>
{
public :
    explicit ItemLoader(bool thumbnailOnly = false);

    Gx::ResourcePtr<Item> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Item> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

    static bool ParseMetadata(Gx::Json json, ItemMetadata& metadata, const Gx::ResourceContext& context);

private:
    bool m_thumbnailOnly = false;
};
