#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/ToggleButton.hpp>

class ToggleButtonLoader : public ResourceLoader<Gx::ToggleButton>
{
public :
    ToggleButtonLoader() = default;

    Gx::ResourcePtr<Gx::ToggleButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::ToggleButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
