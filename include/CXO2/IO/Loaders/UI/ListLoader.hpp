#pragma once

#include <CXO2/IO/ResourceLoader.hpp>

#include <Genode/UI/List.hpp>

namespace Cx
{
    class ListLoader : public ResourceLoader<Gx::List>
    {
    public:
        ListLoader() = default;

        static void OnRegistered(const std::string& id, const Builder& builder);

        [[nodiscard]] Gx::ResourcePtr<Gx::List> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::List> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
