#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/ToolTipMetadata.hpp>
#include <Genode/UI/ToolTip.hpp>

class ToolTipLoader : public ResourceLoader<Gx::ToolTip>
{
public :
    ToolTipLoader() = default;

    Gx::ResourcePtr<Gx::ToolTip> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::ToolTip> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
