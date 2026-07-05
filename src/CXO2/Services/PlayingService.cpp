#include <CXO2/Services/PlayingService.hpp>

#include <CXO2/Contexts/GameContext.hpp>

#include <CXO2/Messages/Requests/ExitWaitingRequest.hpp>

namespace Cx
{
    PlayingOnlineService::PlayingOnlineService(MessageService& messages, GameContext& game) :
        m_messages(messages),
        m_game(game)
    {
    }

    void PlayingOnlineService::ConfirmMusicLoaded(const MessageCallback<ConfirmMusicLoadedRequest>& callback) const
    {
        m_messages.Dispatch(ConfirmMusicLoadedRequest{}, callback);
    }

    void PlayingOnlineService::SubmitScore(
        const SubmitScoreRequest& request,
        const MessageCallback<SubmitScoreRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void PlayingOnlineService::UpdateGameStats(
        const UpdateGameStatsRequest& request,
        const MessageCallback<UpdateGameStatsRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void PlayingOnlineService::ExitPlaying(const MessageCallback<ExitPlayingRequest>& callback) const
    {
        m_messages.Dispatch<ExitPlayingRequest>(ExitPlayingRequest{}, [this, callback] (const MessageEnvelope<ExitPlayingRequest>& result)
        {
            try
            {
                const auto& _ = result.Open();
            }
            catch (...)
            {
                if (callback)
                    callback(std::current_exception());

                return;
            }

            if (m_game.GetMode() != GameMode::Single)
            {
                m_messages.Dispatch<ExitWaitingRequest>(ExitWaitingRequest{}, [callback] (const MessageEnvelope<ExitWaitingRequest>& result)
                {
                    try
                    {
                        const auto& _ = result.Open();

                        if (callback)
                            callback(ExitPlayingRequest{});
                    }
                    catch (...)
                    {
                        if (callback)
                            callback(std::current_exception());
                    }
                });
            }
            else if (callback)
                callback(result);
        });
    }

    void PlayingOnlineService::ConfirmResult(const MessageCallback<ExitPlayingRequest>& callback) const
    {
        m_messages.Dispatch(ExitPlayingRequest{}, callback);
    }

    void PlayingOnlineService::SetMemberMusicLoadedEventCallback(const MessageCallback<MemberMusicLoadedEventData>& callback)
    {
        m_musicLoadedSubscriber = m_messages.On<MemberMusicLoadedEventData>(callback);
    }

    void PlayingOnlineService::SetMemberStatsUpdateEventCallback(const MessageCallback<PlayingMemberStatsUpdateEventData>& callback)
    {
        m_statsSubscriber = m_messages.On<PlayingMemberStatsUpdateEventData>(callback);
    }

    void PlayingOnlineService::SetMemberScoreSubmittedEventCallback(const MessageCallback<PlayingMemberScoreSubmissionEventData>& callback)
    {
        m_scoreSubscriber = m_messages.On<PlayingMemberScoreSubmissionEventData>(callback);
    }

    void PlayingOnlineService::SetMemberLeftEventCallback(const MessageCallback<PlayingMemberLeftEventData>& callback)
    {
        m_leftSubscriber = m_messages.On<PlayingMemberLeftEventData>(callback);
    }

    void PlayingOnlineService::SetGameCompletedEventCallback(const MessageCallback<GameCompletedEventData>& callback)
    {
        m_completedSubscriber = m_messages.On<GameCompletedEventData>(callback);
    }
}
