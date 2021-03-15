#ifndef RADIO_BUTTON_METADATA_LOADER_HPP
#define RADIO_BUTTON_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <O2/IO/Metadata/RadioButtonMetadata.hpp>

class RadioButtonMetadataLoader : public Gx::MetadataLoader<Gx::RadioButton>
{
public :
    RadioButtonMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::RadioButton* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif