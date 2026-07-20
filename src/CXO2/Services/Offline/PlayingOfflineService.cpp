#include <CXO2/Services/Offline/PlayingOfflineService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>

namespace Cx
{
    PlayingOfflineService::PlayingOfflineService(SessionContext& session) :
        m_session(session)
    {
    }

    void PlayingOfflineService::ConfirmMusicLoaded(const MessageCallback<ConfirmMusicLoadedRequest>& callback) const
    {
        if (m_musicLoadedCallback)
            m_musicLoadedCallback(MessageEnvelope<MemberMusicLoadedEventData>(MemberMusicLoadedEventData{ 0 }));

        if (callback)
            callback(MessageEnvelope<ConfirmMusicLoadedRequest>(ConfirmMusicLoadedRequest{}));
    }

    void PlayingOfflineService::SubmitScore(
        const SubmitScoreRequest& request,
        const MessageCallback<SubmitScoreRequest>& callback
    ) const
    {
        auto entry = GameCompletedEventData::ScoreEntry{};
        entry.ID          = 0;
        entry.Active      = 1;
        entry.Cool        = request.Cool;
        entry.Good        = request.Good;
        entry.Bad         = request.Bad;
        entry.Miss        = request.Miss;
        entry.MaxCombo    = request.MaxCombo;
        entry.MaxJamCombo = request.MaxJamCombo;
        entry.Score       = request.Score;
        entry.Level       = m_session.GetLevel();
        entry.Experience  = m_session.GetExperience();
        entry.IsWinning   = true;

        if (m_scoreCallback)
            m_scoreCallback(MessageEnvelope<PlayingMemberScoreSubmissionEventData>(PlayingMemberScoreSubmissionEventData{ 0 }));

        if (m_completedCallback)
            m_completedCallback(MessageEnvelope<GameCompletedEventData>(GameCompletedEventData{ std::vector{ entry } }));

        if (callback)
            callback(MessageEnvelope<SubmitScoreRequest>(request));
    }

    void PlayingOfflineService::UpdateGameStats(
        const UpdateGameStatsRequest& request,
        const MessageCallback<UpdateGameStatsRequest>& callback
    ) const
    {
        if (m_statsCallback)
            m_statsCallback(MessageEnvelope<PlayingMemberStatsUpdateEventData>(PlayingMemberStatsUpdateEventData{ 0, request.Type, request.Value }));

        if (callback)
            callback(MessageEnvelope<UpdateGameStatsRequest>(request));
    }

    void PlayingOfflineService::ExitPlaying(const GameMode, const MessageCallback<ExitPlayingRequest>& callback) const
    {
        if (callback)
            callback(MessageEnvelope<ExitPlayingRequest>(ExitPlayingRequest{}));
    }

    void PlayingOfflineService::ConfirmResult(const MessageCallback<ExitPlayingRequest>& callback) const
    {
        if (callback)
            callback(MessageEnvelope<ExitPlayingRequest>(ExitPlayingRequest{}));
    }

    void PlayingOfflineService::SetMemberMusicLoadedEventCallback(const MessageCallback<MemberMusicLoadedEventData>& callback)
    {
        m_musicLoadedCallback = callback;
    }

    void PlayingOfflineService::SetMemberStatsUpdateEventCallback(const MessageCallback<PlayingMemberStatsUpdateEventData>& callback)
    {
        m_statsCallback = callback;
    }

    void PlayingOfflineService::SetMemberScoreSubmittedEventCallback(const MessageCallback<PlayingMemberScoreSubmissionEventData>& callback)
    {
        m_scoreCallback = callback;
    }

    void PlayingOfflineService::SetMemberLeftEventCallback(const MessageCallback<PlayingMemberLeftEventData>& callback)
    {
        m_leftCallback = callback;
    }

    void PlayingOfflineService::SetGameCompletedEventCallback(const MessageCallback<GameCompletedEventData>& callback)
    {
        m_completedCallback = callback;
    }
}
