#pragma once

#include <CXO2/IO/ResourceLoader.hpp>

#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <CXO2/Avatar/Item.hpp>

namespace Cx
{
    class ItemLoader : public ResourceLoader<Item>
    {
    public :
        explicit ItemLoader(bool thumbnailOnly = false);

        [[nodiscard]] Gx::ResourcePtr<Item> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Item> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

        [[nodiscard]] static bool ParseMetadata(Gx::Json json, ItemMetadata& metadata, const Gx::ResourceContext& context);

    private:
        bool m_thumbnailOnly = false;
    };
}
