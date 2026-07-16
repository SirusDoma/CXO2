#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/States/State.hpp>

namespace Cx
{
    struct StateMetadata;
    class StateLoader : public ResourceLoader<State>
    {
    public :
        explicit StateLoader(Gx::ResourceManager& resources);

        static void OnRegistered(const std::string& id, const Builder& builder);

        [[nodiscard]] bool IsFailSafe() const override { return false; }

        Gx::ResourcePtr<State> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<State> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    private:
        Gx::ResourceManager& m_resources;
    };
}
