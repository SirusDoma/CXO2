#ifndef REPEATER_METADATA_LOADER_HPP
#define REPEATER_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Repeater.hpp>

#include <O2/IO/Metadata/UI/RepeaterMetadata.hpp>
#include <O2/IO/Loaders/TransformLoader.hpp>

class RepeaterLoader : public Gx::MetadataLoader<Gx::Repeater>
{
public :
    RepeaterLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Repeater* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif