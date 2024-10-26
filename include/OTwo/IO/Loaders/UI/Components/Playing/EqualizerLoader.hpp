#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

class EqualizerLoader : public ResourceLoader<Equalizer>
{
public:
    EqualizerLoader() = default;

    Gx::ResourcePtr<Equalizer> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Equalizer> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
