#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/TextBoxMetadata.hpp>
#include <Genode/UI/TextBox.hpp>

class TextBoxLoader : public ResourceLoader<Gx::TextBox>
{
public :
    TextBoxLoader() = default;

    Gx::ResourcePtr<Gx::TextBox> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::TextBox> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
