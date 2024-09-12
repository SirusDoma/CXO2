#ifndef O2JAM_STATE_LOADER_HPP
#define O2JAM_STATE_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/State.hpp>

#include <unordered_map>
#include <typeindex>

struct StateMetadata;
class StateLoader : public ResourceLoader<State>
{
public :
    StateLoader() = default;

    Gx::ResourcePtr<State> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<State> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& ctx) const override;
};

#endif