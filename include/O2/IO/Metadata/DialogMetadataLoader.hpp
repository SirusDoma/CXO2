#ifndef DIALOG_METADATA_LOADER_HPP
#define DIALOG_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Dialog.hpp>

#include <O2/IO/Metadata/DialogMetadata.hpp>

class DialogMetadataLoader : public Gx::MetadataLoader<Gx::Dialog>
{
public :
    DialogMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Dialog* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif