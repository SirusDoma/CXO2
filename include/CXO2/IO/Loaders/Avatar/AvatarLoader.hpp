#pragma once

#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/IO/ResourceLoader.hpp>

namespace Cx
{
    class ItemFactory;
    class AvatarLoader : public ResourceLoader<Avatar>
    {
    public :
        explicit AvatarLoader(ItemFactory& items);

        [[nodiscard]] Gx::ResourcePtr<Avatar> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Avatar> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

    private:
        ItemFactory* m_items;
    };
}
