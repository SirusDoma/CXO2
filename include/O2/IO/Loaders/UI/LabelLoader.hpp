#ifndef LABEL_METADATA_LOADER_HPP
#define LABEL_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Label.hpp>

#include <O2/IO/Metadata/UI/LabelMetadata.hpp>
#include <O2/IO/Loaders/TransformLoader.hpp>

class LabelLoader : public Gx::MetadataLoader<Gx::Label>
{
public :
    LabelLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Label* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;

    static void Parse(Json json, LabelMetadata *metadata);
};

#endif