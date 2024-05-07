#ifndef O2JAM_STATE_PLAYING_LOADER_HPP
#define O2JAM_STATE_PLAYING_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/StatePlaying.hpp>

#include <unordered_map>
#include <typeindex>

struct StateMetadata;
class PlayingResourceContext;
class ObjectPopulator;
class StatePlayingLoader : public ResourceLoader<StatePlaying>
{
public :
    StatePlayingLoader() = default;

    Gx::ResourcePtr<StatePlaying> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<StatePlaying> LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const override;

private:
    static void LoadRequiredResource(ObjectPopulator populator, const StateMetadata *metadata, const std::string &key, const std::string &suffix, const PlayingResourceContext &context, unsigned int count = 1);
};

#endif