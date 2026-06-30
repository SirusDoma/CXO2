#pragma once

#include <CXO2/Services/EventService.hpp>
#include <functional>

namespace Cx
{
    struct ScoreResultEntry;
    struct SubmitScoreRequest;
    struct GameCompletedEventData;

    class NetworkAdapter;
    class NetworkException;
    class GameContext;
    class SessionContext;
    class PlayingService : public virtual Service
    {
    public:
        virtual void ConfirmMusicLoaded(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;

        virtual void SubmitScore(
            const SubmitScoreRequest& score,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;

        virtual void UpdateLife(
            std::uint16_t life,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;

        virtual void UpdateJam(
            std::uint16_t jams,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;

        virtual void ExitPlaying(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;

        virtual void ConfirmResult(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const = 0;
    };

    class PlayingOnlineService : public PlayingService, public EventService
    {
    public:
        PlayingOnlineService(NetworkAdapter& adapter, SessionContext& session, GameContext& game);

        void ConfirmMusicLoaded(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

        void SubmitScore(
            const SubmitScoreRequest& request,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

        void UpdateLife(
            std::uint16_t life,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

        void UpdateJam(
            std::uint16_t jams,
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

        void ExitPlaying(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

        void ConfirmResult(
            std::function<void()> callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) const override;

    private:
        SessionContext& m_session;
        GameContext& m_game;
    };
}
