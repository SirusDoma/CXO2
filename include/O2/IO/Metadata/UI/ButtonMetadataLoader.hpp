#ifndef BUTTON_METADATA_LOADER_HPP
#define BUTTON_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Button.hpp>

#include <O2/IO/Metadata/UI/ButtonMetadata.hpp>

class ButtonMetadataLoader : public Gx::MetadataLoader<Gx::Button>
{
public :
    ButtonMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Button* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;

    static void Parse(Json attributes, std::unordered_map<std::string, Gx::Button::State> stateMap, ButtonMetadata* metadata);
};

#endif