#pragma once

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/IO/ResourceLoader.hpp>

class ItemFactory;
class AvatarLoader : public ResourceLoader<Avatar>
{
public :
    explicit AvatarLoader(ItemFactory& items);

    Gx::ResourcePtr<Avatar> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Avatar> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

    static bool Parse(const Gx::Json& json, ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext::Default);

private:
    ItemFactory* m_items;
};
