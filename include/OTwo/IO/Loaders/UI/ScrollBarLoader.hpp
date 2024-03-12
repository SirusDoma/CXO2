#ifndef O2JAM_SCROLL_BAR_LOADER_HPP
#define O2JAM_SCROLL_BAR_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/ScrollBar.hpp>

class ScrollBarLoader : public ResourceLoader<Gx::ScrollBar>
{
public :
    ScrollBarLoader() = default;

    Gx::ResourcePtr<Gx::ScrollBar> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::ScrollBar> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif