#ifndef O2JAM_CREATE_ROOM_DIALOG_LOADER_HPP
#define O2JAM_CREATE_ROOM_DIALOG_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>

#include <O2/Dialogs/CreateRoomDialog.hpp>
#include <O2/Metadata/UI/DialogMetadata.hpp>

class CreateRoomDialogLoader : public O2JamResourceLoader<CreateRoomDialog>
{
public :
    CreateRoomDialogLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<CreateRoomDialog> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
