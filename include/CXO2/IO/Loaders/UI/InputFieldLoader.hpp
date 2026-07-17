#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/InputFieldMetadata.hpp>
#include <Genode/UI/InputField.hpp>

namespace Cx
{
    class InputFieldLoader : public ResourceLoader<Gx::InputField>
    {
    public :
        InputFieldLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::InputField> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::InputField> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
