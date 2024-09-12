#ifndef O2JAM_LOADER_EQUALIZER_LOADER_HPP
#define O2JAM_LOADER_EQUALIZER_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

class EqualizerLoader : public ResourceLoader<Equalizer>
{
public:
    EqualizerLoader() = default;

    Gx::ResourcePtr<Equalizer> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Equalizer> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};


#endif
