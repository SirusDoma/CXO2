#ifndef O2JAM_PROGRESS_BAR_LOADER_HPP
#define O2JAM_PROGRESS_BAR_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/ProgressBar.hpp>

class ProgressBarLoader : public ResourceLoader<Gx::ProgressBar>
{
public :
    ProgressBarLoader() = default;

    Gx::ResourcePtr<Gx::ProgressBar> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;

    Gx::ResourcePtr<Gx::ProgressBar>
    LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;

};

#endif