#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Application.hpp>
#include <OTwo/Services/NetworkService.hpp>
#include <OTwo/Network/NetworkAdapter.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CommandLineContext.hpp>
#include <OTwo/Contexts/RoomContext.hpp>
#include <OTwo/Messages/RoomInfo.hpp>
#include <OTwo/Messages/Events/AnnouncementEventData.hpp>

#include <OTwo/Models/Planet.hpp>

#include <OTwo/Messages/Requests/PingRequest.hpp>
#include <OTwo/Messages/Responses/PingResponse.hpp>

#include <OTwo/Messages/Events/RoomCreatedEventData.hpp>
#include <OTwo/Messages/Events/RoomMusicChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomStateChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomTitleChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomUserCountChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomRemovedEventData.hpp>

#include <OTwo/Messages/Events/WaitingSlotChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberJoinedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberLeftEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberTeamChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMusicChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingTitleChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMapChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingKickEventData.hpp>
#include <OTwo/Messages/Events/StartGameEventData.hpp>

#include <OTwo/Messages/Events/MemberMusicLoadedEventData.hpp>
#include <OTwo/Messages/Events/PlayingMemberStatsUpdateEventData.hpp>
#include <OTwo/Messages/Events/PlayingMemberScoreSubmissionEventData.hpp>
#include <OTwo/Messages/Events/PlayingMemberLeftEventData.hpp>
#include <OTwo/Messages/Events/GameCompletedEventData.hpp>

#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

OnlineNetworkService::OnlineNetworkService(
    NetworkAdapter& adapter,
    CommandLineContext& args,
    SessionContext& session,
    RoomContext& room
) :
    m_adapter(adapter),
    m_args(args),
    m_session(session),
    m_room(room)
{
}

void OnlineNetworkService::RegisterHandlers()
{
    m_adapter.Subscribe<RoomCreatedEventData>(std::bind(&OnlineNetworkService::OnRoomCreated, this, std::placeholders::_1));
    m_adapter.Subscribe<RoomMusicChangedEventData>(std::bind(&OnlineNetworkService::OnRoomMusicChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<RoomStateChangedEventData>(std::bind(&OnlineNetworkService::OnRoomStateChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<RoomTitleChangedEventData>(std::bind(&OnlineNetworkService::OnRoomTitleChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<RoomUserCountChangedEventData>(std::bind(&OnlineNetworkService::OnRoomUserCountChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<RoomRemovedEventData>(std::bind(&OnlineNetworkService::OnRoomRemoved, this, std::placeholders::_1));

    m_adapter.Subscribe<WaitingSlotChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingSlotChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMemberJoinedEventData>(std::bind(&OnlineNetworkService::OnWaitingMemberJoined, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMemberLeftEventData>(std::bind(&OnlineNetworkService::OnWaitingMemberLeft, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMemberTeamChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingMemberTeamChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMemberReadyStateChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingMemberReadyStateChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMusicChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingMusicChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingTitleChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingTitleChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingMapChangedEventData>(std::bind(&OnlineNetworkService::OnWaitingMapChanged, this, std::placeholders::_1));
    m_adapter.Subscribe<WaitingKickEventData>(std::bind(&OnlineNetworkService::OnKicked, this, std::placeholders::_1));
    m_adapter.Subscribe<StartGameEventData>(std::bind(&OnlineNetworkService::OnStartGame, this, std::placeholders::_1));

    m_adapter.Subscribe<MemberMusicLoadedEventData>(std::bind(&OnlineNetworkService::OnMemberConfirmMusicLoaded, this, std::placeholders::_1));
    m_adapter.Subscribe<PlayingMemberStatsUpdateEventData>(std::bind(&OnlineNetworkService::OnPlayingMemberStatsUpdate, this, std::placeholders::_1));
    m_adapter.Subscribe<PlayingMemberScoreSubmissionEventData>(std::bind(&OnlineNetworkService::OnPlayingMemberScoreSubmitted, this, std::placeholders::_1));
    m_adapter.Subscribe<PlayingMemberLeftEventData>(std::bind(&OnlineNetworkService::OnPlayingMemberLeft, this, std::placeholders::_1));
    m_adapter.Subscribe<GameCompletedEventData>(std::bind(&OnlineNetworkService::OnGameCompleted, this, std::placeholders::_1));

    m_adapter.Subscribe<AnnouncementEventData>(std::bind(&OnlineNetworkService::OnAnnouncement, this, std::placeholders::_1));
}

void OnlineNetworkService::Start(MusicHall server, const std::function<void(bool)> callback)
{
    std::thread([=]
    {
        try
        {
            m_adapter.Stop();

            auto gateways = m_args.GetGatewayInfo();
            const auto it = std::find_if(gateways.begin(), gateways.end(), [server] (const GatewayInfo& info)
            {
                return info.Hall == server;
            });

            if (it == gateways.end())
            {
                callback(false);
                return;
            }

            m_adapter.Start(it->Address, it->Port);
            RegisterHandlers();

            callback(true);
        }
        catch (...)
        {
            callback(false);
        }
    }).detach();
}

void OnlineNetworkService::Stop()
{
}

void OnlineNetworkService::StartHeartbeat()
{
    m_adapter.StartHeartbeat<PingRequest, PingResponse>
    (
        sf::seconds(10),
        nullptr,
        [] (const auto& ex)
        {
            auto* state = dynamic_cast<State*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene());
            state->Invoke([ex, state]
            {
                state->ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [state] (bool)
                {
                    Gx::Application::Instance().GetSceneDirector().Present<StatePlanet>();
                });
            });
        }
    );
}

void OnlineNetworkService::StopHeartbeat()
{
    m_adapter.StopHeartbeat();
}

void OnlineNetworkService::OnRoomCreated(const RoomCreatedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomCreated(ev); });
}

void OnlineNetworkService::OnRoomMusicChanged(const RoomMusicChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomMusicChanged(ev); });
}

void OnlineNetworkService::OnRoomStateChanged(const RoomStateChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomStateChanged(ev); });
}

void OnlineNetworkService::OnRoomTitleChanged(const RoomTitleChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomTitleChanged(ev); });
}

void OnlineNetworkService::OnRoomUserCountChanged(const RoomUserCountChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomUserCountChanged(ev); });
}

void OnlineNetworkService::OnRoomRemoved(const RoomRemovedEventData& ev)
{
    if (auto* state = dynamic_cast<StateRoom*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnRoomRemoved(ev); });
}

void OnlineNetworkService::OnWaitingSlotChanged(const WaitingSlotChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnSlotChanged(ev); });
}

void OnlineNetworkService::OnWaitingTitleChanged(const WaitingTitleChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnTitleChanged(ev); });
}

void OnlineNetworkService::OnWaitingMapChanged(const WaitingMapChangedEventData& ev)
{
    m_room.SetMapID(ev.Map.GetMapID());
    m_room.SetRandomizedMapID(ev.Map.GetRandomizedMap());

    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMapChanged(ev); });
}

void OnlineNetworkService::OnWaitingMusicChanged(const WaitingMusicChangedEventData& ev)
{
    if (auto* state = dynamic_cast<::State*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
    {
        state->Invoke([=]
        {
            std::uint32_t musicID = ev.MusicID;
            if (ev.MusicID > std::numeric_limits<std::uint16_t>::max())
            {
                std::uint8_t randomBit = static_cast<std::uint8_t>((musicID >> 28) & 0xFF);
                constexpr int max = static_cast<int>(LevelCategory::Level1) |
                                    static_cast<int>(LevelCategory::Level2) |
                                    static_cast<int>(LevelCategory::Level3) |
                                    static_cast<int>(LevelCategory::Level4);

                if (randomBit >= 1 && randomBit <= max)
                {
                    m_room.SetRandomLevel(static_cast<LevelCategory>(randomBit));
                    musicID = ev.MusicID & 0xFF;
                }
            }
            else
            {
                m_room.SetRandomLevel(static_cast<LevelCategory>(0));
            }

            const auto& musicList = m_session.GetInstalledMusic();

            // TODO: Get music list header without relying OJNList
            const auto it = std::find_if(musicList.begin(), musicList.end(), [id = musicID] (const auto& header) {
                return id == header.ID;
            });

            m_room.SetMusic(it == musicList.end() ? ChartMetadata{musicID} : *it);
            m_room.SetDifficulty(ev.Difficulty);
            m_room.SetSpeedID(ev.SpeedID);

            if (const auto waiting = dynamic_cast<StateWaiting7K*>(state))
                waiting->OnMusicChanged(ev);
        });
    }
}

void OnlineNetworkService::OnWaitingMemberJoined(const WaitingMemberJoinedEventData& ev)
{
    if (auto* state = dynamic_cast<State*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
    {
        state->Invoke([=]
        {
            auto& slot    = m_room.GetSlot(ev.ID);
            slot.State    = RoomSlotState::Occupied;
            slot.Ready    = ev.Ready;
            slot.Team     = ev.Team;
            slot.IsMaster = false;
            slot.Member   = CharacterInfo
            {
                /* .Name            = */ ev.Name,
                /* .Gender          = */ ev.Gender,
                /* .Role            = */ Role::Normal,
                /* .Level           = */ ev.Level,
                /* .Experience      = */ 0,
                /* .RankStats       = */ {},
                /* .Wallet          = */ {},
                /* .EquippedItemIDs = */ ev.EquippedItemIDs,
                /* .Inventory       = */ {},
                /* .MusicIDs        = */ ev.MusicIDs
            };

            if (const auto waiting = dynamic_cast<StateWaiting7K*>(state))
                waiting->OnMemberJoined(ev);
        });
    }
}

void OnlineNetworkService::OnWaitingMemberLeft(const WaitingMemberLeftEventData& ev)
{
    if (auto* state = dynamic_cast<State*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
    {
        state->Invoke([=]
        {
            if (const auto waiting = dynamic_cast<StateWaiting7K*>(state))
            {
                waiting->OnMemberLeft(ev);
            }
            else if (ev.ID < RoomContext::MaxCapacity)
            {
                auto& slot = m_room.GetSlot(ev.ID);
                slot.State = RoomSlotState::Occupied;
                slot.IsMaster = false;
                slot.Ready = false;
                slot.Member = std::nullopt;

                if (ev.RoomMasterID < RoomContext::MaxCapacity)
                {
                    auto& master = m_room.GetSlot(ev.RoomMasterID);
                    master.IsMaster = true;
                    master.Ready    = true;
                }
            }
        });
    }
}

void OnlineNetworkService::OnWaitingMemberTeamChanged(const WaitingMemberTeamChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberTeamChanged(ev); });
}

void OnlineNetworkService::OnWaitingMemberReadyStateChanged(const WaitingMemberReadyStateChangedEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberReadyStateChanged(ev); });
}

void OnlineNetworkService::OnKicked(const WaitingKickEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnKicked(ev); });
}

void OnlineNetworkService::OnStartGame(const StartGameEventData& ev)
{
    if (auto* state = dynamic_cast<StateWaiting7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnStartGame(ev); });
}

void OnlineNetworkService::OnMemberConfirmMusicLoaded(const MemberMusicLoadedEventData& ev)
{
    if (auto* state = dynamic_cast<StateLoading*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberConfirmMusicLoaded(ev); });
}

void OnlineNetworkService::OnPlayingMemberStatsUpdate(const PlayingMemberStatsUpdateEventData& ev)
{
    if (auto* state = dynamic_cast<StatePlaying7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberStatsUpdate(ev); });
}

void OnlineNetworkService::OnPlayingMemberScoreSubmitted(const PlayingMemberScoreSubmissionEventData& ev)
{
    if (auto* state = dynamic_cast<StatePlaying7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberScoreSubmitted(ev); });
}

void OnlineNetworkService::OnPlayingMemberLeft(const PlayingMemberLeftEventData& ev)
{
    if (auto* state = dynamic_cast<StatePlaying7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnMemberLeft(ev); });
}

void OnlineNetworkService::OnGameCompleted(const GameCompletedEventData& ev)
{
    if (auto* state = dynamic_cast<StatePlaying7K*>(&Gx::Application::Instance().GetSceneDirector().GetPresentingScene()))
        state->Invoke([=] { state->OnGameCompleted(ev); });
}

void OnlineNetworkService::OnAnnouncement(const AnnouncementEventData& ev)
{
    State::Announce(ev.Content);
}
