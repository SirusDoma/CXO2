#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Graphics/ShapeMetadata.hpp>
#include <Genode/Graphics/Shape.hpp>

class ShapeLoader : public ResourceLoader<Gx::Shape>
{
public :
    ShapeLoader() = default;

    Gx::ResourcePtr<Gx::Shape> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Shape> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
