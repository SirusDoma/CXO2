#ifndef O2JAM_SCROLL_BAR_LOADER_HPP
#define O2JAM_SCROLL_BAR_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/ScrollBar.hpp>

class ScrollBarLoader : public O2JamResourceLoader<Gx::ScrollBar>
{
public :
    ScrollBarLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::ScrollBar> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};

#endif