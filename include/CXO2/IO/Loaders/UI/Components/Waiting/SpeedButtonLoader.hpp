#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Components/Waiting/SpeedButton.hpp>

namespace Cx
{
    class SpeedButtonLoader : public ResourceLoader<SpeedButton>
    {
    public:
        SpeedButtonLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<SpeedButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<SpeedButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
