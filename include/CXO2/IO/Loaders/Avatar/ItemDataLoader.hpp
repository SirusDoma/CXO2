#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Avatar/ItemData.hpp>

namespace Cx
{
    class ItemDataLoader : public ResourceLoader<ItemData>
    {
    public :
        ItemDataLoader() = default;

        Gx::ResourcePtr<ItemData> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;

        Gx::ResourcePtr<ItemData> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<ItemData> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
    };
}
