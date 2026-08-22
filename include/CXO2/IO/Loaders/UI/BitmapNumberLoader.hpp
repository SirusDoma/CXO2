#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/BitmapNumber.hpp>

namespace Cx
{
    class BitmapNumberLoader : public ResourceLoader<BitmapNumber>
    {
    public :
        BitmapNumberLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<BitmapNumber> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<BitmapNumber> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
