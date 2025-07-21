#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Avatar/ItemData.hpp>

class ItemDataLoader : public ResourceLoader<ItemData>
{
public :
    ItemDataLoader() = default;

    Gx::ResourcePtr<ItemData> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;

    Gx::ResourcePtr<ItemData> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<ItemData> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};
