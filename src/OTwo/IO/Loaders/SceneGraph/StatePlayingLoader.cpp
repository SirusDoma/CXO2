#include <OTwo/IO/Loaders/SceneGraph/StatePlayingLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/SceneGraph/StateMetadata.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

#include <magic_enum.hpp>
#include <random>
#include <Genode/UI/List.hpp>

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

    auto state = std::make_unique<StatePlaying>();
    state->SetName(meta.Name);

    auto ctx  = static_cast<const PlayingResourceContext&>(Gx::ResourceContext::MakeAvailable(context, state->GetLocalResources()));
    auto maps = std::unordered_set<std::string>();
    for (auto [key, _] : meta.Require)
    {
        if (Gx::StringHelper::StartsWith(key, "IDC_IMAGE_PLAYING_BG"))
            maps.insert(key);
    }

    unsigned int mapID = ctx.GetMapID();
    if (mapID == 0)
    {
        auto device     = std::random_device();
        auto seeder     = std::mt19937(device());
        auto randomizer = std::uniform_int_distribution<unsigned int>(1, maps.size());
        mapID           = randomizer(seeder);
    }

    LoadRequiredResource(ObjectPopulator::Decorate(state.get(), false), metadata, "IDC_IMAGE_PLAYING_BG", std::to_string(mapID), ctx);
    LoadRequiredResource(ObjectPopulator::Decorate(state.get(), false), metadata, "IDC_IMAGE_NOTE_BG",    std::to_string(mapID), ctx);

    for (int i = 1; i <= 7; i++)
    {
        LoadRequiredResource(ObjectPopulator::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_NORMAL", std::to_string(i), ctx, 1);
        LoadRequiredResource(ObjectPopulator::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_LONG",   std::to_string(i), ctx, 1);
    }

    auto populator = ObjectPopulator::Decorate(state.get());
    for (auto [key, object] : metadata->Objects)
    {
        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetLocalResources(), context.GetCacheMode());

        ObjectLoader::Load(name, object, populator, ctx);
    }

    if (auto keyEffectContainer = state->FindChild<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT"); keyEffectContainer)
    {
        LoadRequiredResource(ObjectPopulator::Decorate(keyEffectContainer), metadata, "IDC_IMAGE_KEY_EFFECT", std::to_string(mapID), ctx, 7);
        keyEffectContainer->SetBatchingEnabled(true);
    }
    else
        throw Gx::ResourceAccessException("IDC_CONTAINER_KEY_EFFECT");

    if (auto noteJamContainer = state->FindChild<Gx::UiContainer>("IDC_CONTAINER_NOTE_JAM"); noteJamContainer)
        noteJamContainer->SetVisible(false);
    else
        throw Gx::ResourceAccessException("IDC_CONTAINER_NOTE_JAM");

    if (auto comboContainer = state->FindChild<Gx::UiContainer>("IDC_CONTAINER_COMBO"); comboContainer)
    {
        auto suffix = std::string();
        if (ctx.IsFxEnabled())
            suffix = std::to_string(mapID);

        LoadRequiredResource(ObjectPopulator::Decorate(comboContainer), metadata, "IDC_ANIMATION_NOTE_COMBO", suffix, ctx);
        LoadRequiredResource(ObjectPopulator::Decorate(comboContainer), metadata, "IDC_NUMBER_NOTE_COMBO", suffix, ctx);
        comboContainer->SetVisible(false);
    }
    else
        throw Gx::ResourceAccessException("IDC_CONTAINER_COMBO");

    if (auto noteClickList = state->FindChild<Gx::List>("IDC_LIST_NOTE_CLICK"); noteClickList)
    {
        LoadRequiredResource(ObjectPopulator::Decorate(noteClickList),  metadata, "IDC_ANIMATION_NOTE_CLICK", std::to_string(mapID) + "_" + std::to_string(ctx.GetEffectID()), ctx, 7);
        for (auto child :noteClickList->GetChildren())
        {
            if (auto animation = dynamic_cast<Gx::Animation*>(child); animation)
            {
                for (auto i = 0; i < animation->GetFrameCount(); i++)
                {
                    auto &frame = animation->GetFrame(i);
                    frame.Position = animation->GetPosition();
                    frame.Origin = { frame.TexCoords.width / 2.f, frame.TexCoords.height / 2.f };
                }
            }
        }

        noteClickList->SetBatchingEnabled(true);
    }
    else
        throw Gx::ResourceAccessException("IDC_LIST_NOTE_CLICK");

    return state;
}

void StatePlayingLoader::LoadRequiredResource(ObjectPopulator populator, const StateMetadata *metadata, const std::string &key, const std::string &suffix, const PlayingResourceContext &context, const unsigned int count)
{
    if (const auto it = metadata->Require.find(key + suffix); it != metadata->Require.end())
    {
        const auto name      = populator.GetName() + "/" + key;
        const auto reference = std::any_cast<Gx::Json>(it->second);
        auto ctx             = Gx::ResourceContext::Rebind(name, context);

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
