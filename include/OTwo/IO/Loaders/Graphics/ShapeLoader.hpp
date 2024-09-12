#ifndef O2JAM_SHAPE_LOADER_HPP
#define O2JAM_SHAPE_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Graphics/ShapeMetadata.hpp>
#include <Genode/Graphics/Shape.hpp>

class ShapeLoader : public ResourceLoader<Gx::Shape>
{
public :
    ShapeLoader() = default;

    Gx::ResourcePtr<Gx::Shape> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Shape> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};

#endif