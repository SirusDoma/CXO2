#ifndef O2JAM_DIALOG_LOADER_HPP
#define O2JAM_DIALOG_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>

#include <Genode/UI/Dialog.hpp>
#include <OTwo/Metadata/UI/DialogMetadata.hpp>

class DialogLoader : public ResourceLoader<Gx::Dialog>
{
public :
    DialogLoader() = default;

    Gx::ResourcePtr<Gx::Dialog> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Dialog> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};

#endif