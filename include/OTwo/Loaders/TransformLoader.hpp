#ifndef O2JAM_TRANSFORM_LOADER_HPP
#define O2JAM_TRANSFORM_LOADER_HPP

#include <SFML/Graphics/Transform.hpp>
#include <OTwo/Loaders/O2JamResourceLoader.hpp>

class TransformMetadata;
class TransformLoader : public O2JamResourceLoader<sf::Transform>
{
public :
    TransformLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<sf::Transform> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

    static void ParseTransform(Json attributes, TransformMetadata &metadata);
};

#endif