#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Animation.hpp>

class AnimationLoader : public ResourceLoader<Gx::Animation>
{
public :
    AnimationLoader() = default;

    Gx::ResourcePtr<Gx::Animation> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Animation> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
