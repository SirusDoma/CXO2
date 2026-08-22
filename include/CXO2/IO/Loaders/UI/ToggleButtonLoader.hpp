#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <CXO2/UI/ToggleButton.hpp>

namespace Cx
{
    class ToggleButtonLoader : public ResourceLoader<ToggleButton>
    {
    public :
        ToggleButtonLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<ToggleButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<ToggleButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
