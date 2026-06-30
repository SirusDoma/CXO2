#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace Cx
{
    class TransformMetadata;
    class TransformLoader : public ResourceLoader<sf::Transform>
    {
    public :
        TransformLoader() = default;

        Gx::ResourcePtr<sf::Transform> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<sf::Transform> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& transform, TransformMetadata& metadata, const Gx::ResourceContext& ctx = Gx::ResourceContext::Default);
    };
}
