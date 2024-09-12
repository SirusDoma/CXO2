#ifndef O2JAM_TRANSFORM_LOADER_HPP
#define O2JAM_TRANSFORM_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Transform.hpp>

class TransformMetadata;
class TransformLoader : public ResourceLoader<sf::Transform>
{
public :
    TransformLoader() = default;

    Gx::ResourcePtr<sf::Transform> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::Transform> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

    static bool ParseMetadata(Gx::Json transform, TransformMetadata& metadata, const Gx::ResourceContext& ctx = Gx::ResourceContext::Default);
};

#endif