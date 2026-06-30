#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/ToggleButton.hpp>

namespace Cx
{
    class ToggleButtonLoader : public ResourceLoader<Gx::ToggleButton>
    {
    public :
        ToggleButtonLoader() = default;

        Gx::ResourcePtr<Gx::ToggleButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::ToggleButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
