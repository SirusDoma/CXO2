#include <OTwo/IO/Loaders/SceneGraph/StatePlaying7KLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/Metadata/SceneGraph/StatePlayingMetadata.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/UI/List.hpp>

#include <magic_enum.hpp>
#include <random>

Gx::ResourcePtr<StatePlaying7K> StatePlaying7KLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    StatePlayingMetadata metadata;
    if (json.find("name") == json.end())
        throw Gx::ResourceLoadException("State must have a name");

    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    const auto attributes = json.at("attributes");
    metadata.Viewport = attributes.at("viewport").get<unsigned int>();

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<StatePlaying7K> StatePlaying7KLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const StatePlayingMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto state = std::make_unique<StatePlaying7K>();
    state->SetName(meta.Name);
    state->SetViewport(metadata->Viewport);

    auto ctx  = static_cast<const PlayingResourceContext&>(Gx::ResourceContext::MakeAvailable(context, state->GetResources()));
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

    LoadRequiredResource(ObjectContainer::Decorate(state.get(), false), metadata, "IDC_IMAGE_PLAYING_BG", std::to_string(mapID), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), false), metadata, "IDC_IMAGE_NOTE_BG",    std::to_string(mapID), ctx);

    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_IMAGE_PLAYING_MENU",   std::string(), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_LIST_NOTE_CLICK",      std::string(), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_LIST_LONG_NOTE_EFFECT",std::string(), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_IMAGE_NOTE_MEASURE1",  std::string(), ctx, 1);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_IMAGE_NOTE_MEASURE2",  std::string(), ctx, 1);

    for (int i = 1; i <= 7; i++) // Channel
    {
        for (int s = 1; s <= 2; s++) // Shape (Square, Circle)
        {
            LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_NORMAL" + std::to_string(i) + "_" + std::to_string(s), std::string(), ctx, 1);
            LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_LONG"   + std::to_string(i) + "_" + std::to_string(s), std::string(), ctx, 1);
        }
    }

    auto sfxSuffix = std::string();
    if (ctx.IsFxEnabled())
        sfxSuffix = std::to_string(mapID);

    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_COMBO", sfxSuffix, ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_NUMBER_NOTE_COMBO", sfxSuffix, ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_EFFECT_JAM", sfxSuffix, ctx);

    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_COOL", sfxSuffix, ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_GOOD", sfxSuffix, ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_BAD", sfxSuffix, ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_ANIMATION_NOTE_MISS", sfxSuffix, ctx);

    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_NUMBER_EFFECT_JAM", std::string(), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_CONTAINER_NOTE_JAM", std::string(), ctx);
    LoadRequiredResource(ObjectContainer::Decorate(state.get(), true), metadata, "IDC_BUTTON_EXIT", std::string(), ctx);

    auto container = ObjectContainer::Decorate(state.get());
    for (auto [key, object] : metadata->Objects)
    {
        // Rewire resource manager to the local scene
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext(name, state->GetResources(), context.GetCacheMode());

        ObjectLoader::Load(name, object, container, ctx);
    }

    if (auto keyEffectContainer = state->FindChild<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT"); keyEffectContainer)
    {
        LoadRequiredResource(ObjectContainer::Decorate(keyEffectContainer), metadata, "IDC_IMAGE_KEY_EFFECT", std::to_string(mapID), ctx, 7);
        keyEffectContainer->SetBatchingEnabled(true);
    }
    else
        throw Gx::ResourceAccessException("IDC_CONTAINER_KEY_EFFECT");

    if (auto longNoteEffectList = state->FindResource<Gx::List>("IDC_LIST_LONG_NOTE_EFFECT"); longNoteEffectList)
    {
        for (auto child :longNoteEffectList->GetChildren())
        {
            if (auto animation = dynamic_cast<Gx::Animation*>(child); animation)
            {
                for (auto i = 0; i < animation->GetFrameCount(); i++)
                {
                    auto &frame    = animation->GetFrame(i);
                    frame.Position = animation->GetPosition();
                    frame.Origin   = animation->GetOrigin();
                }
            }
        }

        longNoteEffectList->SetBatchingEnabled(true);
    }
    else
        throw Gx::ResourceAccessException("IDC_LIST_LONG_NOTE_EFFECT");

    if (auto noteClickList = state->FindResource<Gx::List>("IDC_LIST_NOTE_CLICK"); noteClickList)
    {
        auto noteClickSuffix = std::string();
        if (ctx.IsFxEnabled())
            noteClickSuffix = std::to_string(mapID) + "_" + std::to_string(ctx.GetEffectID());

        LoadRequiredResource(ObjectContainer::Decorate(noteClickList),  metadata, "IDC_ANIMATION_NOTE_CLICK", noteClickSuffix, ctx, 7);
        for (auto child :noteClickList->GetChildren())
        {
            if (auto animation = dynamic_cast<Gx::Animation*>(child); animation)
            {
                for (auto i = 0; i < animation->GetFrameCount(); i++)
                {
                    auto &frame = animation->GetFrame(i);
                    frame.Position = animation->GetPosition();
                    if (frame.TexCoords.width == animation->GetOrigin().x * 2 && frame.TexCoords.height == animation->GetOrigin().y * 2)
                        frame.Origin = animation->GetOrigin();
                }
            }
        }

        noteClickList->SetBatchingEnabled(true);
    }
    else
        throw Gx::ResourceAccessException("IDC_LIST_NOTE_CLICK");

    return state;
}

void StatePlaying7KLoader::LoadRequiredResource(ObjectContainer container, const StateMetadata *metadata, const std::string &key, const std::string &suffix, const PlayingResourceContext &context, const unsigned int count)
{
    if (const auto it = metadata->Require.find(key + suffix); it != metadata->Require.end())
    {
        const auto name      = container.GetName() + "/" + key;
        const auto reference = std::any_cast<Gx::Json>(it->second);
        auto ctx             = Gx::ResourceContext::Rebind(name, context);

        if (count > 1)
        {
            for (auto i = 1; i <= count; i++)
                ObjectLoader::Load(name + std::to_string(i), reference, container, ctx);
        }
        else
            ObjectLoader::Load(name, reference, container, ctx);
    }
    else
        throw Gx::Exception(key + " is not found within required resource references");
}
