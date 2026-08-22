#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/ToolTipMetadata.hpp>
#include <CXO2/UI/ToolTip.hpp>

namespace Cx
{
    class ToolTipLoader : public ResourceLoader<ToolTip>
    {
    public :
        ToolTipLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<ToolTip> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<ToolTip> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
