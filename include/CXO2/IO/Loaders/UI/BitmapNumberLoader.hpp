#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <Genode/UI/BitmapNumber.hpp>

namespace Cx
{
    class BitmapNumberLoader : public ResourceLoader<Gx::BitmapNumber>
    {
    public :
        BitmapNumberLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::BitmapNumber> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::BitmapNumber> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
