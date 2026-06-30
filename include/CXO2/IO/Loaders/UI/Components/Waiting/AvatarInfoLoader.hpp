#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Waiting/AvatarInfo.hpp>

namespace Cx
{
    class AvatarInfoLoader : public ResourceLoader<AvatarInfo>
    {
    public :
        AvatarInfoLoader() = default;

        Gx::ResourcePtr<AvatarInfo> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<AvatarInfo> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
