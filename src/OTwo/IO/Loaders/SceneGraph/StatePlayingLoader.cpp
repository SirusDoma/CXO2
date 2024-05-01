#include <OTwo/IO/Loaders/SceneGraph/StatePlayingLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/SceneGraph/StateMetadata.hpp>

#include <Genode/UI.hpp>
#include <magic_enum.hpp>
#include <random>
#include <OTwo/IO/PlayingResourceContext.hpp>

Gx::ResourcePtr<StatePlaying> StatePlayingLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    StateMetadata metadata;
    if (json.find("name") == json.end())
        throw Gx::ResourceLoadException("State must have a name.");

    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<StatePlaying> StatePlayingLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const StateMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto ctx = dynamic_cast<const PlayingResourceContext*>(&context);
    if (!ctx)
        return nullptr;

    auto state     = std::make_unique<StatePlaying>();
    auto populator = ObjectPopulator::Decorate(state.get());
    state->SetName(meta.Name);

    auto included = std::unordered_set<std::string>();

    const std::string mapPrefix     = "IDC_IMAGE_PLAYING_BG";
    const std::string overlayPrefix = "IDC_IMAGE_NOTE_BG";

    auto maps     = std::unordered_set<std::string>();
    auto overlays = std::unordered_set<std::string>();
    for (auto [key, _] : meta.Require)
    {
        if (key.rfind(mapPrefix, 0) != std::string::npos)
            maps.insert(key);

        if (key.rfind(overlayPrefix, 0) != std::string::npos)
            overlays.insert(key);
    }

    unsigned int mapID = ctx->GetMapID();
    if (mapID == 0)
    {
        auto device     = std::random_device();
        auto seeder     = std::mt19937(device());
        auto randomizer = std::uniform_int_distribution<unsigned int>(1, maps.size());
        mapID = randomizer(seeder);
    }

    const std::string playingBg = mapPrefix + std::to_string(mapID);
    included.insert(playingBg);
    if (const auto it = maps.find(playingBg); it == maps.end())
        throw Gx::Exception("Invalid Playing BG Map ID: " + std::to_string(ctx->GetMapID()));

    const std::string noteBg = overlayPrefix + std::to_string(mapID);
    included.insert(noteBg);
    if (const auto it = overlays.find(noteBg); it == overlays.end())
        throw Gx::Exception("Invalid Note BG Map ID: " + std::to_string(ctx->GetMapID()));

    for (auto [key, value] : meta.Require)
    {
        if (const auto it = included.find(key); it == included.end())
            continue;

        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        ObjectLoader::Load(name, reference, populator, ctx);
    }

    for (auto [key, object] : metadata->Objects)
    {
        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        ObjectLoader::Load(name, object, populator, ctx);
    }

    return state;
}
