#ifndef TRANSFORM_METADATA_LOADER_HPP
#define TRANSFORM_METADATA_LOADER_HPP

#include <SFML/Graphics/Transform.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <O2/IO/Metadata/TransformMetadata.hpp>

class TransformMetadataLoader : public Gx::MetadataLoader<sf::Transform>
{
public :
    TransformMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual sf::Transform* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;

    static void Parse(Json attributes, TransformMetadata* metadata);
};

#endif