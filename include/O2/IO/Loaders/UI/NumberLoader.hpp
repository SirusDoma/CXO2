#ifndef NUMBER_METADATA_LOADER_HPP
#define NUMBER_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Number.hpp>

#include <O2/IO/Metadata/UI/NumberMetadata.hpp>

class NumberLoader : public Gx::MetadataLoader<Gx::Number>
{
public :
    NumberLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Number* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif