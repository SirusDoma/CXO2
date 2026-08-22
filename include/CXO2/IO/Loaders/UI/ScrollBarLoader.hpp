#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/ScrollBar.hpp>

namespace Cx
{
    class ScrollBarLoader : public ResourceLoader<ScrollBar>
    {
    public :
        ScrollBarLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<ScrollBar> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<ScrollBar> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
