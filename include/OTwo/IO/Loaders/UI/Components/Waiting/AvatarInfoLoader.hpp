#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

class AvatarInfoLoader : public ResourceLoader<AvatarInfo>
{
public :
    AvatarInfoLoader() = default;

    Gx::ResourcePtr<AvatarInfo> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<AvatarInfo> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
