#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Gauge.hpp>

namespace Cx
{
    class GaugeLoader : public ResourceLoader<Gauge>
    {
    public :
        GaugeLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gauge> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gauge> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
