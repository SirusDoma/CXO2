#include <OTwo/Services/PlayingService.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <OTwo/Network/NetworkAdapter.hpp>

#include <OTwo/Messages/Requests/ExitWaitingRequest.hpp>
#include <OTwo/Messages/Requests/ConfirmMusicLoadedRequest.hpp>
#include <OTwo/Messages/Requests/ExitPlayingRequest.hpp>
#include <OTwo/Messages/Requests/SubmitScoreRequest.hpp>
#include <OTwo/Messages/Requests/UpdateGameStatsRequest.hpp>

#include <OTwo/Messages/Events/PlayingMemberStatsUpdateEventData.hpp>


PlayingOnlineService::PlayingOnlineService(NetworkAdapter& adapter, SessionContext& session, GameContext& game) :
    EventService(adapter),
    m_session(session),
    m_game(game)
{
}

void PlayingOnlineService::ConfirmMusicLoaded(
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<ConfirmMusicLoadedRequest>(
        ConfirmMusicLoadedRequest{},
        callback,
        errorCallback
    );
}

void PlayingOnlineService::SubmitScore(
    const SubmitScoreRequest& request,
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<SubmitScoreRequest>(
        request,
        callback,
        errorCallback
    );
}

void PlayingOnlineService::UpdateLife(
    const std::uint16_t life,
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<UpdateGameStatsRequest>(
        UpdateGameStatsRequest{ UpdateStatsType::Life, life },
        callback,
        errorCallback
    );
}

void PlayingOnlineService::UpdateJam(
    const std::uint16_t jams,
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<UpdateGameStatsRequest>(
        UpdateGameStatsRequest{ UpdateStatsType::Jam, jams },
        callback,
        errorCallback
    );
}

void PlayingOnlineService::ExitPlaying(
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<ExitPlayingRequest>(
        ExitPlayingRequest{},
        [=]
        {
            if (m_game.GetMode() != GameMode::Single)
                GetNetworkAdapter().SendAsync<ExitWaitingRequest>(ExitWaitingRequest{}, callback, errorCallback);
            else if (callback)
                callback();
        },
        errorCallback
    );
}

void PlayingOnlineService::ConfirmResult(
    const std::function<void()> callback,
    const std::function<void(const NetworkException&)>& errorCallback
) const
{
    GetNetworkAdapter().SendAsync<ExitPlayingRequest>(
        ExitPlayingRequest{},
        callback,
        errorCallback
    );
}
