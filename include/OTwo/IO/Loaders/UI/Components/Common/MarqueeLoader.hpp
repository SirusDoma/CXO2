#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Common/Marquee.hpp>

class MarqueeLoader : public ResourceLoader<Marquee>
{
public :
    MarqueeLoader() = default;

    Gx::ResourcePtr<Marquee> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Marquee> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
