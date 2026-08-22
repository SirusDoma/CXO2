#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/InputFieldMetadata.hpp>
#include <CXO2/UI/InputField.hpp>

namespace Cx
{
    class InputFieldLoader : public ResourceLoader<InputField>
    {
    public :
        InputFieldLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<InputField> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<InputField> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
