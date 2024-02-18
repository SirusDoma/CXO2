#ifndef O2JAM_MARQUEE_LOADER_HPP
#define O2JAM_MARQUEE_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>

#include <OTwo/Metadata/UI/Components/MarqueeMetadata.hpp>
#include <OTwo/States/Components/Room/Marquee.hpp>

class MarqueeLoader : public O2JamResourceLoader<Marquee>
{
public :
    MarqueeLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Marquee> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif