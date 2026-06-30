#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/RadioButton.hpp>

namespace Cx
{
    class RadioButtonLoader : public ResourceLoader<Gx::RadioButton>
    {
    public:
        RadioButtonLoader() = default;

        Gx::ResourcePtr<Gx::RadioButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::RadioButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
