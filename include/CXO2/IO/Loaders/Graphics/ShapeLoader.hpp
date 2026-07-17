#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Graphics/ShapeMetadata.hpp>
#include <Genode/Graphics/Shape.hpp>

namespace Cx
{
    class ShapeLoader : public ResourceLoader<Gx::Shape>
    {
    public :
        ShapeLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::Shape> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Shape> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
