#ifndef O2JAM_BUTTON_LOADER_HPP
#define O2JAM_BUTTON_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/ButtonMetadata.hpp>

#include <Genode/UI/Button.hpp>

class ButtonLoader : public ResourceLoader<Gx::Button>
{
public :
    ButtonLoader() = default;

    Gx::ResourcePtr<Gx::Button> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::Button> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;

    static bool ParseMetadata(const Gx::Json &json, ButtonMetadata &metadata, const Gx::ResourceContext &context);
};

#endif