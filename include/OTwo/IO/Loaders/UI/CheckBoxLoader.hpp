#ifndef O2JAM_CHECK_BOX_LOADER_HPP
#define O2JAM_CHECK_BOX_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/CheckBox.hpp>

class CheckBoxLoader : public ResourceLoader<Gx::CheckBox>
{
public :
    CheckBoxLoader() = default;

    Gx::ResourcePtr<Gx::CheckBox> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;

    Gx::ResourcePtr<Gx::CheckBox>
    LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};


#endif