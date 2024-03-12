#ifndef O2JAM_UI_CONTAINER_LOADER_HPP
#define O2JAM_UI_CONTAINER_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>

#include <Genode/UI/UiContainer.hpp>

class UiContainerLoader : public ResourceLoader<Gx::UiContainer>
{
public :
    UiContainerLoader() = default;

    Gx::ResourcePtr<Gx::UiContainer> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::UiContainer> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
