#ifndef IMAGE_METADATA_LOADER_HPP
#define IMAGE_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/Image.hpp>

#include <O2/IO/Metadata/UI/ImageMetadata.hpp>

class ImageMetadataLoader : public Gx::MetadataLoader<Gx::Image>
{
public :
    ImageMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Image* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif