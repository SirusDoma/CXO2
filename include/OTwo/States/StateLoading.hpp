#pragma once

#include <OTwo/States/State.hpp>

#include <OTwo/Core/Chart.hpp>
#include <OTwo/IO/Loaders/Chart/O2JamChartLoader.hpp>

#include <unordered_set>

struct WaitingMemberLeftEventData;
struct MemberMusicLoadedEventData;

class PlayingService;
class GameContext;
class RoomContext;
class SessionContext;
class StateLoading : public State
{
public:
    StateLoading(GameContext& game, SessionContext& session, RoomContext& room, PlayingService& service);

    void Initialize() override;

    static bool IsTrackable();

    void OnMemberConfirmMusicLoaded(const MemberMusicLoadedEventData& ev);
    void OnMemberLeft(const WaitingMemberLeftEventData& ev);

private:
    void OnCoverLoaded(const sf::Image* cover);
    void OnChartLoaded(const Chart* chart);

    sf::Texture m_texture;
    GameContext& m_context;
    SessionContext& m_session;
    RoomContext& m_room;
    PlayingService& m_service;
    std::condition_variable m_signal;
    std::mutex m_mutex;
    std::unordered_set<std::uint8_t> m_loadedUsers;
};
