#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Animation.hpp>

namespace Cx
{
    class AnimationLoader : public ResourceLoader<Gx::Animation>
    {
    public :
        AnimationLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::Animation> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Animation> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
