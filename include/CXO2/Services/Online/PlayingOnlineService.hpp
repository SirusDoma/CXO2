#pragma once

#include <CXO2/Services/PlayingService.hpp>
#include <CXO2/Services/NetworkService.hpp>

namespace Cx
{
    class PlayingOnlineService : public PlayingService
    {
    public:
        explicit PlayingOnlineService(NetworkService& network);

        void ConfirmMusicLoaded(
            const MessageCallback<ConfirmMusicLoadedRequest>& callback = nullptr
        ) const override;

        void SubmitScore(
            const SubmitScoreRequest& request,
            const MessageCallback<SubmitScoreRequest>& callback = nullptr
        ) const override;

        void UpdateGameStats(
            const UpdateGameStatsRequest& request,
            const MessageCallback<UpdateGameStatsRequest>& callback = nullptr
        ) const override;

        void ExitPlaying(
            GameMode mode,
            const MessageCallback<ExitPlayingRequest>& callback = nullptr
        ) const override;

        void ConfirmResult(
            const MessageCallback<ExitPlayingRequest>& callback = nullptr
        ) const override;

        void SetMemberMusicLoadedEventCallback(
            const MessageCallback<MemberMusicLoadedEventData>& callback
        ) override;

        void SetMemberStatsUpdateEventCallback(
            const MessageCallback<PlayingMemberStatsUpdateEventData>& callback
        ) override;

        void SetMemberScoreSubmittedEventCallback(
            const MessageCallback<PlayingMemberScoreSubmissionEventData>& callback
        ) override;

        void SetMemberLeftEventCallback(
            const MessageCallback<PlayingMemberLeftEventData>& callback
        ) override;

        void SetGameCompletedEventCallback(
            const MessageCallback<GameCompletedEventData>& callback
        ) override;

    private:
        NetworkService& m_network;

        MessageSubscriber<MemberMusicLoadedEventData>            m_musicLoadedSubscriber;
        MessageSubscriber<PlayingMemberStatsUpdateEventData>     m_statsSubscriber;
        MessageSubscriber<PlayingMemberScoreSubmissionEventData> m_scoreSubscriber;
        MessageSubscriber<PlayingMemberLeftEventData>            m_leftSubscriber;
        MessageSubscriber<GameCompletedEventData>                m_completedSubscriber;
    };
}
