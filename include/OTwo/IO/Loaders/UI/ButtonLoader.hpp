#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/ButtonMetadata.hpp>

#include <Genode/UI/Button.hpp>

class ButtonLoader : public ResourceLoader<Gx::Button>
{
public :
    ButtonLoader() = default;

    Gx::ResourcePtr<Gx::Button> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Button> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    static bool ParseMetadata(const Gx::Json& json, ButtonMetadata& metadata, const Gx::ResourceContext& context);
};
