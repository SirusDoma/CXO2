#ifndef O2JAM_OPTION_DIALOG_LOADER_HPP
#define O2JAM_OPTION_DIALOG_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>

#include <OTwo/Dialogs/OptionDialog.hpp>
#include <OTwo/Metadata/UI/DialogMetadata.hpp>

class OptionDialogLoader : public O2JamResourceLoader<OptionDialog>
{
public :
    OptionDialogLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<OptionDialog> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
