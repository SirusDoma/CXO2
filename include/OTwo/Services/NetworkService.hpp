#pragma once

#include <OTwo/Services/Service.hpp>

#include <cstdint>
#include <functional>

struct AnnouncementEventData;
enum class MusicHall : std::uint8_t;
enum class StartGameResult : std::uint32_t;

struct AuthResponse;
struct ChannelListResponse;
struct ChannelLoginResponse;

struct RoomCreatedEventData;
struct RoomMusicChangedEventData;
struct RoomStateChangedEventData;
struct RoomTitleChangedEventData;
struct RoomUserCountChangedEventData;
struct RoomRemovedEventData;

struct WaitingMapChangedEventData;
struct WaitingTitleChangedEventData;
struct WaitingMusicChangedEventData;
struct WaitingMemberReadyStateChangedEventData;
struct WaitingMemberTeamChangedEventData;
struct WaitingMemberLeftEventData;
struct WaitingMemberJoinedEventData;
struct WaitingSlotChangedEventData;
struct WaitingKickEventData;
struct StartGameEventData;

struct MemberMusicLoadedEventData;
struct PlayingMemberStatsUpdateEventData;
struct PlayingMemberScoreSubmissionEventData;
struct PlayingMemberLeftEventData;
struct GameCompletedEventData;

class NetworkAdapter;
class NetworkException;
class CommandLineContext;
class SessionContext;
class RoomContext;
class NetworkService : public Service
{
public:
    virtual void RegisterHandlers() = 0;

    virtual void Start(MusicHall server, std::function<void(bool)> callback) = 0;
    virtual void Stop() = 0;

    virtual void StartHeartbeat() = 0;
    virtual void StopHeartbeat() = 0;

    virtual void OnRoomCreated(const RoomCreatedEventData&) = 0;
    virtual void OnRoomMusicChanged(const RoomMusicChangedEventData&) = 0;
    virtual void OnRoomStateChanged(const RoomStateChangedEventData&) = 0;
    virtual void OnRoomTitleChanged(const RoomTitleChangedEventData&) = 0;
    virtual void OnRoomUserCountChanged(const RoomUserCountChangedEventData&) = 0;
    virtual void OnRoomRemoved(const RoomRemovedEventData&) = 0;

    virtual void OnWaitingSlotChanged(const WaitingSlotChangedEventData&) = 0;
    virtual void OnWaitingTitleChanged(const WaitingTitleChangedEventData&) = 0;
    virtual void OnWaitingMapChanged(const WaitingMapChangedEventData&) = 0;
    virtual void OnWaitingMusicChanged(const WaitingMusicChangedEventData&) = 0;
    virtual void OnWaitingMemberJoined(const WaitingMemberJoinedEventData&) = 0;
    virtual void OnWaitingMemberLeft(const WaitingMemberLeftEventData&) = 0;
    virtual void OnWaitingMemberTeamChanged(const WaitingMemberTeamChangedEventData&) = 0;
    virtual void OnWaitingMemberReadyStateChanged(const WaitingMemberReadyStateChangedEventData&) = 0;
    virtual void OnKicked(const WaitingKickEventData&) = 0;
    virtual void OnStartGame(const StartGameEventData&) = 0;

    virtual void OnMemberConfirmMusicLoaded(const MemberMusicLoadedEventData&) = 0;

    virtual void OnPlayingMemberStatsUpdate(const PlayingMemberStatsUpdateEventData&) = 0;
    virtual void OnPlayingMemberScoreSubmitted(const PlayingMemberScoreSubmissionEventData&) = 0;
    virtual void OnPlayingMemberLeft(const PlayingMemberLeftEventData&) = 0;
    virtual void OnGameCompleted(const GameCompletedEventData&) = 0;

    virtual void OnAnnouncement(const AnnouncementEventData&) = 0;
};

class OnlineNetworkService : public NetworkService
{
public:
    explicit OnlineNetworkService(
        NetworkAdapter& adapter,
        CommandLineContext& args,
        SessionContext& session,
        RoomContext& room
    );

    void RegisterHandlers() override;

    void Start(MusicHall server, std::function<void(bool)> callback) override;
    void Stop() override;

    void StartHeartbeat() override;
    void StopHeartbeat() override;

    void OnRoomCreated(const RoomCreatedEventData&) override;
    void OnRoomMusicChanged(const RoomMusicChangedEventData&) override;
    void OnRoomStateChanged(const RoomStateChangedEventData&) override;
    void OnRoomTitleChanged(const RoomTitleChangedEventData&) override;
    void OnRoomUserCountChanged(const RoomUserCountChangedEventData&) override;
    void OnRoomRemoved(const RoomRemovedEventData&) override;

    void OnWaitingSlotChanged(const WaitingSlotChangedEventData&) override;
    void OnWaitingMemberJoined(const WaitingMemberJoinedEventData&) override;
    void OnWaitingMemberLeft(const WaitingMemberLeftEventData&) override;
    void OnWaitingMemberTeamChanged(const WaitingMemberTeamChangedEventData&) override;
    void OnWaitingMemberReadyStateChanged(const WaitingMemberReadyStateChangedEventData&) override;
    void OnWaitingMusicChanged(const WaitingMusicChangedEventData&) override;
    void OnWaitingTitleChanged(const WaitingTitleChangedEventData&) override;
    void OnWaitingMapChanged(const WaitingMapChangedEventData&) override;
    void OnKicked(const WaitingKickEventData&) override;
    void OnStartGame(const StartGameEventData&) override;

    void OnMemberConfirmMusicLoaded(const MemberMusicLoadedEventData&) override;

    void OnPlayingMemberStatsUpdate(const PlayingMemberStatsUpdateEventData&) override;
    void OnPlayingMemberScoreSubmitted(const PlayingMemberScoreSubmissionEventData&) override;
    void OnPlayingMemberLeft(const PlayingMemberLeftEventData&) override;
    void OnGameCompleted(const GameCompletedEventData&) override;

    void OnAnnouncement(const AnnouncementEventData&) override;

private:
    NetworkAdapter& m_adapter;
    CommandLineContext& m_args;
    SessionContext& m_session;
    RoomContext& m_room;
};
