#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/InputFieldMetadata.hpp>
#include <Genode/UI/InputField.hpp>

class InputFieldLoader : public ResourceLoader<Gx::InputField>
{
public :
    InputFieldLoader() = default;

    Gx::ResourcePtr<Gx::InputField> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::InputField> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
