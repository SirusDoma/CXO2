#ifndef O2JAM_AVATAR_LOADER_HPP
#define O2JAM_AVATAR_LOADER_HPP

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/IO/ResourceLoader.hpp>

class AvatarLoader : public ResourceLoader<Avatar>
{
public :
    AvatarLoader() = default;

    Gx::ResourcePtr<Avatar> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Avatar> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
