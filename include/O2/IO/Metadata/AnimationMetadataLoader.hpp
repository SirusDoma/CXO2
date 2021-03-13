#ifndef ANIMATION_METADATA_LOADER_HPP
#define ANIMATION_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Animation.hpp>

#include <O2/IO/Metadata/AnimationMetadata.hpp>

#include <memory>

class AnimationMetadataLoader : public Gx::MetadataLoader<Gx::Animation>
{
public :
    AnimationMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Animation* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif