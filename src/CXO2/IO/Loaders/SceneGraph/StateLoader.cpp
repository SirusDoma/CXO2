#include <CXO2/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/IO/TextureCacheBuilder.hpp>
#include <CXO2/Metadata/SceneGraph/StateMetadata.hpp>

#include <CXO2/States/StateAvi.hpp>
#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateMusicShop.hpp>
#include <CXO2/States/StateItemShop.hpp>
#include <CXO2/States/StateMyRoom.hpp>
#include <CXO2/States/StateBulletin.hpp>
#include <CXO2/States/StatePayment.hpp>
#include <CXO2/States/StateWaiting7K.hpp>
#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StateResult.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Contexts/CartContext.hpp>
#include <CXO2/Services/MessageService.hpp>
#include <CXO2/Services/AuthService.hpp>
#include <CXO2/Services/PlanetService.hpp>
#include <CXO2/Services/ChatService.hpp>
#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/Services/CharacterService.hpp>
#include <CXO2/Services/WaitingService.hpp>
#include <CXO2/Services/PlayingService.hpp>
#include <CXO2/Services/ItemShopService.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>
#include <CXO2/Core/ScoreTracker.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    StateLoader::StateLoader(Gx::ResourceManager& resources)
        : m_resources(resources)
    {
    }

    void StateLoader::OnRegistered(const std::string&, const Builder&)
    {
        Gx::ResourceLoaderFactory::Map<State,
            StateAvi,
            StatePlanet,
            StateRoom,
            StateMusicShop,
            StateItemShop,
            StateMyRoom,
            StateBulletin,
            StatePayment,
            StateWaiting7K,
            StateLoading,
            StateResult
        >();
    }

    Gx::ResourcePtr<State> StateLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const
    {
        StateMetadata metadata;
        if (json.find("name") == json.end())
            throw Gx::ResourceLoadException(ctx.GetID(), "State must have a name");

        if (!MetadataLoader::Parse(json, metadata, ctx))
            return nullptr;

        return LoadFromMetadata(metadata, ctx);
    }

    Gx::ResourcePtr<State> StateLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        auto metadata = dynamic_cast<const StateMetadata*>(&meta);
        if (metadata == nullptr)
            return nullptr;

        auto state = Instantiate(context);
        state->SetName(meta.Name);

        auto load = [&] (const std::string& key, const Gx::Json& object, const bool instantiate)
        {
            auto composer = SceneComposer::Compose(*state, instantiate);
            auto name = fmt::format("{}/{}", meta.Name, key);
            auto ctx  = Gx::ResourceContext(name, m_resources, context.GetCacheMode());
            for (const auto& [property, value] : context.GetProperties())
                ctx.SetProperty(property, value);

            composer.Add(name, object, ctx);
        };

        for (const auto& [key, reference] : meta.Require)
        {
            if (reference.is_string())
                load(key, reference, false);
        }

        for (const auto& [key, object] : metadata->Objects)
            load(key, object, true);

        return state;
    }
}
