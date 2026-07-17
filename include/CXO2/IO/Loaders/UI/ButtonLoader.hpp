#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/ButtonMetadata.hpp>

#include <Genode/UI/Button.hpp>

namespace Cx
{
    class ButtonLoader : public ResourceLoader<Gx::Button>
    {
    public :
        ButtonLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::Button> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Button> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& attributes, ButtonMetadata& metadata, const Gx::ResourceContext& context);
    };
}
