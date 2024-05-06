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

    auto maps       = std::unordered_set<std::string>();
    for (auto [key, _] : meta.Require)
    {
        if (Gx::StringHelper::StartsWith(key, "IDC_IMAGE_PLAYING_BG"))
            maps.insert(key);
    }

    unsigned int mapID = playingCtx->GetMapID();
    if (mapID == 0)
    {
        auto device     = std::random_device();
        auto seeder     = std::mt19937(device());
        auto randomizer = std::uniform_int_distribution<unsigned int>(1, maps.size());
        mapID           = randomizer(seeder);
    }

    LoadRequiredResource(state.get(), metadata, "IDC_IMAGE_PLAYING_BG",     std::to_string(mapID), context);
    LoadRequiredResource(state.get(), metadata, "IDC_IMAGE_NOTE_BG",        std::to_string(mapID), context);
    LoadRequiredResource(state.get(), metadata, "IDC_IMAGE_KEY_EFFECT",     std::to_string(mapID), context, 7);
    LoadRequiredResource(state.get(), metadata, "IDC_ANIMATION_NOTE_CLICK", std::to_string(mapID) + "_" + std::to_string(playingCtx->GetEffectID()), context, 7, true);

    for (int i = 1; i <= 7; i++)
    {
        LoadRequiredResource(state.get(), metadata, "IDC_ANIMATION_NOTE_NORMAL", std::to_string(i), context, 1, true);
        LoadRequiredResource(state.get(), metadata, "IDC_ANIMATION_NOTE_LONG",   std::to_string(i), context, 1, true);
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

void StatePlayingLoader::LoadRequiredResource(StatePlaying *state, const StateMetadata *metadata, const std::string &key, const std::string &suffix, const Gx::ResourceContext &context, const unsigned int count, const bool importOnly)
{
    if (const auto it = metadata->Require.find(key + suffix); it != metadata->Require.end())
    {
        const auto name      = metadata->Name + "/" + key;
        const auto reference = std::any_cast<Gx::Json>(it->second);
        auto populator       = ObjectPopulator::Decorate(state, importOnly);
        auto ctx             = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        if (count > 1)
        {
            for (auto i = 1; i <= count; i++)
                ObjectLoader::Load(name + std::to_string(i), reference, populator, ctx);
        }
        else
            ObjectLoader::Load(name, reference, populator, ctx);
    }
    else
        throw Gx::Exception(key + " is not found within required resource references");
}
