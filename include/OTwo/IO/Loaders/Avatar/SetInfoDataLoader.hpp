#ifndef O2JAM_SET_INFO_DATA_LOADER_HPP
#define O2JAM_SET_INFO_DATA_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>

struct SetInfoData;
class SetInfoDataLoader : public ResourceLoader<SetInfoData>
{
public :
    SetInfoDataLoader() = default;

    Gx::ResourcePtr<SetInfoData> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<SetInfoData> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};

#endif