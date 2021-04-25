#ifndef O2JAM_TOOLTIP_LOADER_HPP
#define O2JAM_TOOLTIP_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>
#include <O2/Metadata/UI/ToolTipMetadata.hpp>
#include <Genode/UI/ToolTip.hpp>

class ToolTipLoader : public O2JamResourceLoader<Gx::ToolTip>
{
public :
    ToolTipLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::ToolTip> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif