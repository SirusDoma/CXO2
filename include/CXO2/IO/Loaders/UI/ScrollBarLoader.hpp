#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <Genode/UI/ScrollBar.hpp>

namespace Cx
{
    class ScrollBarLoader : public ResourceLoader<Gx::ScrollBar>
    {
    public :
        ScrollBarLoader() = default;

        Gx::ResourcePtr<Gx::ScrollBar> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::ScrollBar> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
