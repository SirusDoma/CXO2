#ifndef O2JAM_ANIMATION_LOADER_HPP
#define O2JAM_ANIMATION_LOADER_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Animation.hpp>

#include <OTwo/Loaders/O2JamResourceLoader.hpp>

class AnimationLoader : public O2JamResourceLoader<Gx::Animation>
{
public :
    AnimationLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Animation> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif