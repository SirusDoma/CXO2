#ifndef O2JAM_RADIO_BUTTON_LOADER_HPP
#define O2JAM_RADIO_BUTTON_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/RadioButton.hpp>

class RadioButtonLoader : public ResourceLoader<Gx::RadioButton>
{
public:
    RadioButtonLoader() = default;

    Gx::ResourcePtr<Gx::RadioButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::RadioButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};

#endif