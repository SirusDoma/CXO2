#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Playing/Equalizer.hpp>

namespace Cx
{
    class EqualizerLoader : public ResourceLoader<Equalizer>
    {
    public:
        EqualizerLoader() = default;

        Gx::ResourcePtr<Equalizer> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Equalizer> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
