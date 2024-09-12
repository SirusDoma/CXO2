#ifndef O2JAM_GAUGE_LOADER_HPP
#define O2JAM_GAUGE_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/Gauge.hpp>

class GaugeLoader : public ResourceLoader<Gx::Gauge>
{
public :
    GaugeLoader() = default;

    Gx::ResourcePtr<Gx::Gauge> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Gauge> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};

#endif