#ifndef O2JAM_AVATAR_INFO_LOADER_HPP
#define O2JAM_AVATAR_INFO_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

class AvatarInfoLoader : public ResourceLoader<AvatarInfo>
{
public :
    AvatarInfoLoader() = default;

    Gx::ResourcePtr<AvatarInfo> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<AvatarInfo> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
