#pragma once

#include <CXO2/IO/ResourceLoader.hpp>

#include <Genode/UI/UiContainer.hpp>

namespace Cx
{
    class UiContainerLoader : public ResourceLoader<Gx::UiContainer>
    {
    public :
        UiContainerLoader() = default;

        static void OnRegistered(const std::string& id, const Builder& builder);

        [[nodiscard]] Gx::ResourcePtr<Gx::UiContainer> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::UiContainer> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
