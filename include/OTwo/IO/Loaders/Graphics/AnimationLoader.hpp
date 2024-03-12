#ifndef O2JAM_ANIMATION_LOADER_HPP
#define O2JAM_ANIMATION_LOADER_HPP


#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Animation.hpp>


class AnimationLoader : public ResourceLoader<Gx::Animation>
{
public :
    AnimationLoader() = default;

    Gx::ResourcePtr<Gx::Animation> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::Animation> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif