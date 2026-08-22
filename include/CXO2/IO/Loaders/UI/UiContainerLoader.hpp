#pragma once

#include <CXO2/IO/ResourceLoader.hpp>

#include <CXO2/UI/UiContainer.hpp>

namespace Cx
{
    class UiContainerLoader : public ResourceLoader<UiContainer>
    {
    public :
        UiContainerLoader() = default;

        static void OnRegistered(const std::string& id, const Builder& builder);

        [[nodiscard]] Gx::ResourcePtr<UiContainer> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<UiContainer> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
