#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/State.hpp>

#include <unordered_map>
#include <typeindex>

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
