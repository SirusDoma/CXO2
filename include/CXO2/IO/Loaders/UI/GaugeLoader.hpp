#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <Genode/UI/Gauge.hpp>

namespace Cx
{
    class GaugeLoader : public ResourceLoader<Gx::Gauge>
    {
    public :
        GaugeLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::Gauge> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Gauge> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
