#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/ButtonMetadata.hpp>

#include <CXO2/UI/Button.hpp>

namespace Cx
{
    class ButtonLoader : public ResourceLoader<Button>
    {
    public :
        ButtonLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Button> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Button> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& attributes, ButtonMetadata& metadata, const Gx::ResourceContext& context);
    };
}
