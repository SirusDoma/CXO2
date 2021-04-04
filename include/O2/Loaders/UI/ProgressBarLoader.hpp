#ifndef O2JAM_PROGRESS_BAR_LOADER_HPP
#define O2JAM_PROGRESS_BAR_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/ProgressBar.hpp>

class ProgressBarLoader : public O2JamResourceLoader<Gx::ProgressBar>
{
public :
    ProgressBarLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::ProgressBar> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};

#endif