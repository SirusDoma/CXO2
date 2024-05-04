#include <OTwo/IO/Loaders/SceneGraph/StatePlayingLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/SceneGraph/StateMetadata.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

#include <magic_enum.hpp>
#include <random>

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

    auto playingCtx = dynamic_cast<const PlayingResourceContext*>(&context);
    if (!playingCtx)
        throw Gx::NotSupportedException("Context must be a PlayingResourceContext");

    auto state = std::make_unique<StatePlaying>();
    state->SetName(meta.Name);

    auto included = std::unordered_set<std::string>();

    const std::string mapPrefix       = "IDC_IMAGE_PLAYING_BG";
    const std::string overlayPrefix   = "IDC_IMAGE_NOTE_BG";
    const std::string noteClickPrefix = "IDC_ANIMATION_NOTE_CLICK";
    const std::string keyEffectPrefix = "IDC_IMAGE_KEY_EFFECT";

    auto maps       = std::unordered_set<std::string>();
    auto overlays   = std::unordered_set<std::string>();
    auto clicks     = std::unordered_set<std::string>();
    auto keyEffects = std::unordered_set<std::string>();
    for (auto [key, _] : meta.Require)
    {
        if (Gx::StringHelper::StartsWith(key, mapPrefix))
            maps.insert(key);

        if (Gx::StringHelper::StartsWith(key, overlayPrefix))
            overlays.insert(key);

        if (Gx::StringHelper::StartsWith(key, noteClickPrefix))
            clicks.insert(key);

        if (Gx::StringHelper::StartsWith(key, keyEffectPrefix))
            keyEffects.insert(key);
    }

    unsigned int mapID = playingCtx->GetMapID();
    if (mapID == 0)
    {
        auto device     = std::random_device();
        auto seeder     = std::mt19937(device());
        auto randomizer = std::uniform_int_distribution<unsigned int>(1, maps.size());
        mapID           = randomizer(seeder);
    }

    const std::string playingBg = mapPrefix + std::to_string(mapID);
    included.insert(playingBg);
    if (const auto it = maps.find(playingBg); it == maps.end())
        throw Gx::Exception("Invalid Playing BG Map ID: " + std::to_string(playingCtx->GetMapID()));

    const std::string noteBg = overlayPrefix + std::to_string(mapID);
    included.insert(noteBg);
    if (const auto it = overlays.find(noteBg); it == overlays.end())
        throw Gx::Exception("Invalid Note BG Map ID: " + std::to_string(playingCtx->GetMapID()));

    const std::string noteClick = noteClickPrefix + std::to_string(mapID) + "_" + std::to_string(playingCtx->GetEffectID());
    if (const auto it = clicks.find(noteClick); it == clicks.end())
        throw Gx::Exception("Invalid Note Click Map ID: " + std::to_string(playingCtx->GetMapID()) + " Effect ID: " + std::to_string(playingCtx->GetEffectID()));

    const std::string keyEffect = keyEffectPrefix + std::to_string(mapID);
    included.insert(keyEffect);
    if (const auto it = keyEffects.find(keyEffect); it == keyEffects.end())
        throw Gx::Exception("Invalid Key Effect Map ID: " + std::to_string(playingCtx->GetMapID()));

    const std::string normalNotePrefix = "IDC_ANIMATION_NOTE_NORMAL";
    const std::string longNotePrefix   = "IDC_ANIMATION_NOTE_LONG";
    for (auto [key, value] : meta.Require)
    {
        auto reference = std::any_cast<Gx::Json>(value);
        if (reference.type() != Gx::Json::value_t::string)
            continue;

        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        auto populator = ObjectPopulator::Decorate(state.get());
        if (key == noteClick)
        {
            populator = ObjectPopulator::Decorate(state.get(), true);
            for (auto i = 1; i <= 7; i++)
            {
                name = meta.Name + "/" + noteClickPrefix + std::to_string(i);
                ObjectLoader::Load(name, reference, populator, ctx);
            }
        }
        else if (key == keyEffect)
        {
            populator = ObjectPopulator::Decorate(state.get(), true);
            for (auto i = 1; i <= 7; i++)
            {
                name = meta.Name + "/" + keyEffectPrefix + std::to_string(i);
                ObjectLoader::Load(name, reference, populator, ctx);
            }
        }
        else
        {
            if (const auto it = included.find(key); it == included.end())
                continue;

            if (Gx::StringHelper::StartsWith(meta.Name, normalNotePrefix) || Gx::StringHelper::StartsWith(meta.Name, longNotePrefix))
                populator = ObjectPopulator::Decorate(state.get(), true);

            ObjectLoader::Load(name, reference, populator, ctx);
        }
    }

    auto populator = ObjectPopulator::Decorate(state.get());
    for (auto [key, object] : metadata->Objects)
    {
        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        ObjectLoader::Load(name, object, populator, ctx);
    }

    return state;
}
