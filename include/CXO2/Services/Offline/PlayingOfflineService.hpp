#pragma once

#include <CXO2/Services/PlayingService.hpp>

namespace Cx
{
    class SessionContext;
    class PlayingOfflineService : public PlayingService
    {
    public:
        explicit PlayingOfflineService(SessionContext& session);

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
        SessionContext& m_session;

        MessageCallback<MemberMusicLoadedEventData>            m_musicLoadedCallback;
        MessageCallback<PlayingMemberStatsUpdateEventData>     m_statsCallback;
        MessageCallback<PlayingMemberScoreSubmissionEventData> m_scoreCallback;
        MessageCallback<PlayingMemberLeftEventData>            m_leftCallback;
        MessageCallback<GameCompletedEventData>                m_completedCallback;
    };
}
