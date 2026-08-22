#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Components/Marquee.hpp>

namespace Cx
{
    class MarqueeLoader : public ResourceLoader<Marquee>
    {
    public :
        MarqueeLoader() = default;

        Gx::ResourcePtr<Marquee> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Marquee> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
