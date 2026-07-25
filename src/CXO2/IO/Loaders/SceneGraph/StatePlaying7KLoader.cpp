#include <CXO2/IO/Loaders/SceneGraph/StatePlaying7KLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Metadata/SceneGraph/StatePlayingMetadata.hpp>
#include <CXO2/IO/PlayingResourceContext.hpp>

#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Constants/Identifiers/Playing7K.hpp>

#include <Genode/UI/List.hpp>

#include <random>

namespace Cx
{
    using namespace Constants::Identifiers;

    StatePlaying7KLoader::StatePlaying7KLoader(Gx::ResourceManager& resources) :
        m_resources(resources)
    {
    }

    Gx::ResourcePtr<StatePlaying7K> StatePlaying7KLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
    {
        StatePlayingMetadata metadata;
        if (json.find("name") == json.end())
            throw Gx::ResourceLoadException(ctx.GetID(), "State must have a name");

        if (!MetadataLoader::Parse(json, metadata, ctx))
            return nullptr;

        const auto attributes = json.at("attributes");
        metadata.Viewport = attributes.at("viewport").get<unsigned int>();

        return LoadFromMetadata(metadata, ctx);
    }

    Gx::ResourcePtr<StatePlaying7K> StatePlaying7KLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        auto metadata = dynamic_cast<const StatePlayingMetadata*>(&meta);
        if (metadata == nullptr)
            return nullptr;

        auto state = Instantiate(context);
        state->SetName(meta.Name);
        state->SetViewport(metadata->Viewport);

        auto ctx = static_cast<const PlayingResourceContext&>(context);
        ctx.Bind(m_resources);

        auto maps = std::unordered_set<std::string>();
        for (auto [key, _] : meta.Require)
        {
            if (Gx::StringHelper::StartsWith(key, Resource::Playing7K::Require::IDC_IMAGE_PLAYING_BG))
                maps.insert(key);
        }

        unsigned int mapID = ctx.GetMapID();
        if (mapID == 0 || mapID > maps.size())
        {
            auto device     = std::random_device();
            auto seeder     = std::mt19937(device());
            auto randomizer = std::uniform_int_distribution<unsigned int>(1, maps.size() - 1);
            mapID           = randomizer(seeder);

            ctx.SetMapID(mapID);
        }

        if (ctx.GetMode() == GameMode::Tutorial)
        {
            LoadRequiredResource(SceneComposer::Compose(*state, true), metadata, Resource::Playing7K::Require::IDC_IMAGE_PLAYING_BG, std::string(), ctx);
            LoadRequiredResource(SceneComposer::Compose(*state, true), metadata, Resource::Playing7K::Require::IDC_IMAGE_NOTE_BG,    std::string(), ctx);
        }
        else
        {
            LoadRequiredResource(SceneComposer::Compose(*state, true), metadata, Resource::Playing7K::Require::IDC_IMAGE_PLAYING_BG, std::to_string(mapID), ctx);
            LoadRequiredResource(SceneComposer::Compose(*state, true), metadata, Resource::Playing7K::Require::IDC_IMAGE_NOTE_BG,    std::to_string(mapID), ctx);
        }

        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_LIST_NOTE_CLICK,      std::string(), ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_LIST_LONG_NOTE_EFFECT,std::string(), ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_IMAGE_NOTE_MEASURE1,  std::string(), ctx, 1);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_IMAGE_NOTE_MEASURE2,  std::string(), ctx, 1);

        for (unsigned int i = 1; i <= 7; i++) // Channel
        {
            for (unsigned int s = 0; s <= 1; s++) // Shape (Circle, Square)
            {
                LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, fmt::format("{}{}_{}", Resource::Playing7K::Require::IDC_ANIMATION_NOTE_NORMAL, i, s), std::string(), ctx, 1);
                LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, fmt::format("{}{}_{}", Resource::Playing7K::Require::IDC_ANIMATION_NOTE_LONG  , i, s), std::string(), ctx, 1);
            }
        }

        auto sfxSuffix = std::string();
        if (ctx.IsFxEnabled())
            sfxSuffix = std::to_string(mapID);

        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_COMBO, sfxSuffix, ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_NUMBER_NOTE_COMBO, sfxSuffix, ctx);

        if (metadata->Require.find(fmt::format("{}{}", Resource::Playing7K::Require::IDC_ANIMATION_EFFECT_JAM, sfxSuffix)) != metadata->Require.end())
            LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_EFFECT_JAM, sfxSuffix, ctx);

        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_COOL, sfxSuffix, ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_GOOD, sfxSuffix, ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_BAD, sfxSuffix, ctx);
        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_MISS, sfxSuffix, ctx);

        if (metadata->Require.find(Resource::Playing7K::Require::IDC_NUMBER_EFFECT_JAM) != metadata->Require.end())
            LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_NUMBER_EFFECT_JAM, std::string(), ctx);

        LoadRequiredResource(SceneComposer::Compose(*state, false), metadata, Resource::Playing7K::Require::IDC_CONTAINER_NOTE_JAM, std::string(), ctx);

        auto composer = SceneComposer::Compose(*state);
        for (const auto& [key, object] : metadata->Objects)
        {
            // Rewire resource manager to the local scene
            auto name = fmt::format("{}/{}", meta.Name, key);
            auto rctx = Gx::ResourceContext(name, m_resources, ctx.GetCacheMode());
            for (const auto& [key, value] : ctx.GetProperties())
                rctx.SetProperty(key, value);

            composer.Add(name, object, rctx);
        }

        if (auto keyEffectContainer = state->FindChild<Gx::UiContainer>(Resource::Playing7K::Require::IDC_CONTAINER_KEY_EFFECT); keyEffectContainer)
        {
            if (ctx.GetMode() == GameMode::Tutorial)
            {
                LoadRequiredResource(SceneComposer::Compose(*keyEffectContainer), metadata, Resource::Playing7K::Require::IDC_IMAGE_KEY_EFFECT, std::string(), ctx, 7);
                LoadRequiredResource(SceneComposer::Compose(*keyEffectContainer), metadata, Resource::Playing7K::Require::IDC_IMAGE_GUIDE_KEY_EFFECT, std::string(), ctx, 7);
            }
            else
                LoadRequiredResource(SceneComposer::Compose(*keyEffectContainer), metadata, Resource::Playing7K::Require::IDC_IMAGE_KEY_EFFECT, std::to_string(mapID), ctx, 7);

            keyEffectContainer->SetBatchingEnabled(true);
        }
        else
            throw Gx::ResourceAccessException(Resource::Playing7K::Require::IDC_CONTAINER_KEY_EFFECT);

        if (auto longNoteEffectList = state->Find<Gx::List>(Resource::Playing7K::Require::IDC_LIST_LONG_NOTE_EFFECT); longNoteEffectList)
        {
            for (auto child :longNoteEffectList->GetChildren())
            {
                if (auto animation = dynamic_cast<Gx::Animation*>(child); animation)
                {
                    for (auto i = 0; i < animation->GetFrameCount(); i++)
                    {
                        auto& frame    = animation->GetFrame(i);
                        frame.Position = animation->GetPosition();
                        frame.Origin   = animation->GetOrigin();
                    }
                }
            }

            longNoteEffectList->SetBatchingEnabled(true);
        }
        else
            throw Gx::ResourceAccessException(Resource::Playing7K::Require::IDC_LIST_LONG_NOTE_EFFECT);

        if (auto noteClickList = state->Find<Gx::List>(Resource::Playing7K::Require::IDC_LIST_NOTE_CLICK); noteClickList)
        {
            auto noteClickSuffix = std::string();
            if (ctx.IsFxEnabled())
                noteClickSuffix = fmt::format("{}_{}", mapID, ctx.GetEffectID());

            if (noteClickList->GetChildrenCount() == 0)
                LoadRequiredResource(SceneComposer::Compose(*noteClickList),  metadata, Resource::Playing7K::Require::IDC_ANIMATION_NOTE_CLICK, noteClickSuffix, ctx, 7);

            for (auto child :noteClickList->GetChildren())
            {
                if (auto animation = dynamic_cast<Gx::Animation*>(child); animation)
                {
                    for (auto i = 0; i < animation->GetFrameCount(); i++)
                    {
                        auto& frame    = animation->GetFrame(i);
                        frame.Position = animation->GetPosition();

                        if (!frame.Origin.has_value())
                            frame.Origin = animation->GetOrigin();
                    }
                }
            }

            noteClickList->SetBatchingEnabled(true);
        }
        else
            throw Gx::ResourceAccessException(Resource::Playing7K::Require::IDC_LIST_NOTE_CLICK);

        return state;
    }

    void StatePlaying7KLoader::LoadRequiredResource(SceneComposer composer, const StateMetadata* metadata, const std::string& key, const std::string& suffix, const PlayingResourceContext& context, const unsigned int count)
    {
        if (const auto it = metadata->Require.find(key + suffix); it != metadata->Require.end())
        {
            const auto name      = fmt::format("{}/{}", composer.GetName(), key);
            const auto reference = it->second;
            auto ctx             = Gx::ResourceContext::Rebind(context, name);

            for (const auto& [key, value] : context.GetProperties())
                ctx.SetProperty(key, value);

            if (count > 1)
            {
                for (auto i = 1; i <= count; i++)
                    composer.Add(fmt::format("{}{}", name, i), reference, ctx);
            }
            else
                composer.Add(name, reference, ctx);
        }
        else
            throw Gx::Exception(fmt::format("{} is not found within required resource references", key));
    }
}
