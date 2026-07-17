#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Avatar/SetInfoData.hpp>

namespace Cx
{
    class SetInfoDataLoader : public ResourceLoader<SetInfoData>
    {
    public :
        SetInfoDataLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<SetInfoData> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<SetInfoData> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<SetInfoData> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
    };
}
