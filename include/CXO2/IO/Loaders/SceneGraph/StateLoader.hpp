#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/States/State.hpp>

#include <unordered_map>
#include <typeindex>

namespace Cx
{
    struct StateMetadata;
    class StateLoader : public ResourceLoader<State>
    {
    public :
        explicit StateLoader(Gx::ResourceManager& resources);

        Gx::ResourcePtr<State> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<State> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    private:
        Gx::ResourceManager& m_resources;
    };
}
