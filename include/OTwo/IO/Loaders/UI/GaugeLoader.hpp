#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/Gauge.hpp>

class GaugeLoader : public ResourceLoader<Gx::Gauge>
{
public :
    GaugeLoader() = default;

    Gx::ResourcePtr<Gx::Gauge> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Gauge> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
