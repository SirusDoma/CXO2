#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/ToolTipMetadata.hpp>
#include <Genode/UI/ToolTip.hpp>

namespace Cx
{
    class ToolTipLoader : public ResourceLoader<Gx::ToolTip>
    {
    public :
        ToolTipLoader() = default;

        Gx::ResourcePtr<Gx::ToolTip> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::ToolTip> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
