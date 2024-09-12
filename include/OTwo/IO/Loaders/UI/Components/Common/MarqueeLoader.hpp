#ifndef O2JAM_MARQUEE_LOADER_HPP
#define O2JAM_MARQUEE_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>

#include <OTwo/Metadata/UI/Components/Common/MarqueeMetadata.hpp>
#include <OTwo/UI/Common/Marquee.hpp>

class MarqueeLoader : public ResourceLoader<Marquee>
{
public :
    MarqueeLoader() = default;

    Gx::ResourcePtr<Marquee> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Marquee> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};

#endif