#pragma once

#include <CXO2/States/State.hpp>

#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Core/Chart.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <unordered_set>
#include <condition_variable>

namespace Cx
{
    struct WaitingMemberLeftEventData;
    class RoomContext;
    class SessionContext;
    class StateLoading : public State
    {
    public:
        StateLoading(GameConfig& config, SessionContext& session, RoomContext& room, PlayingService& service);

        void Initialize() override;
        void Initialize(GameContext game);

        static bool IsTrackable();

        void OnMemberConfirmMusicLoaded(const MessageEnvelope<MemberMusicLoadedEventData>& envelope);
        void OnMemberLeft(const WaitingMemberLeftEventData& ev);

    private:
        void OnCoverLoaded(const sf::Image* cover);
        void OnChartLoaded(const Chart* chart);

        sf::Texture m_texture;
        GameConfig& m_config;
        GameContext m_context;
        SessionContext& m_session;
        RoomContext& m_room;
        PlayingService& m_service;
        std::condition_variable m_signal;
        std::mutex m_mutex;
        std::unordered_set<std::uint8_t> m_loadedUsers;
    };
}
