#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <CXO2/UI/RadioButton.hpp>

namespace Cx
{
    class RadioButtonLoader : public ResourceLoader<RadioButton>
    {
    public:
        RadioButtonLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<RadioButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<RadioButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
