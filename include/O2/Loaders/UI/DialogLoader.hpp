#ifndef O2JAM_DIALOG_LOADER_HPP
#define O2JAM_DIALOG_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/Dialog.hpp>

class DialogLoader : public O2JamResourceLoader<Gx::Dialog>
{
public :
    DialogLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Dialog> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif