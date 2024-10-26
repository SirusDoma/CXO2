#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/StatePlaying7K.hpp>

struct StateMetadata;
class PlayingResourceContext;
class ObjectContainer;
class StatePlaying7KLoader : public ResourceLoader<StatePlaying7K>
{
public :
    StatePlaying7KLoader() = default;

    Gx::ResourcePtr<StatePlaying7K> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<StatePlaying7K> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

private:
    static void LoadRequiredResource(ObjectContainer container, const StateMetadata* metadata, const std::string& key, const std::string& suffix, const PlayingResourceContext& context, unsigned int count = 1);
};
