#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>

#include <CXO2/Network/Requests/ConfirmMusicLoadedRequest.hpp>
#include <CXO2/Network/Requests/ExitPlayingRequest.hpp>
#include <CXO2/Network/Requests/SubmitScoreRequest.hpp>
#include <CXO2/Network/Requests/UpdateGameStatsRequest.hpp>

#include <CXO2/Network/Events/MemberMusicLoadedEventData.hpp>
#include <CXO2/Network/Events/PlayingMemberStatsUpdateEventData.hpp>
#include <CXO2/Network/Events/PlayingMemberScoreSubmissionEventData.hpp>
#include <CXO2/Network/Events/PlayingMemberLeftEventData.hpp>
#include <CXO2/Network/Events/GameCompletedEventData.hpp>

#include <CXO2/Models/Game.hpp>

#include <functional>

namespace Cx
{
    class PlayingService : public virtual Service
    {
    public:
        virtual void ConfirmMusicLoaded(
            const MessageCallback<ConfirmMusicLoadedRequest>& callback = nullptr
        ) const = 0;

        virtual void SubmitScore(
            const SubmitScoreRequest& request,
            const MessageCallback<SubmitScoreRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateGameStats(
            const UpdateGameStatsRequest& request,
            const MessageCallback<UpdateGameStatsRequest>& callback = nullptr
        ) const = 0;

        virtual void ExitPlaying(
            GameMode mode,
            const MessageCallback<ExitPlayingRequest>& callback = nullptr
        ) const = 0;

        virtual void ConfirmResult(
            const MessageCallback<ExitPlayingRequest>& callback = nullptr
        ) const = 0;

        virtual void SetMemberMusicLoadedEventCallback(
            const MessageCallback<MemberMusicLoadedEventData>& callback
        ) = 0;

        virtual void SetMemberStatsUpdateEventCallback(
            const MessageCallback<PlayingMemberStatsUpdateEventData>& callback
        ) = 0;

        virtual void SetMemberScoreSubmittedEventCallback(
            const MessageCallback<PlayingMemberScoreSubmissionEventData>& callback
        ) = 0;

        virtual void SetMemberLeftEventCallback(
            const MessageCallback<PlayingMemberLeftEventData>& callback
        ) = 0;

        virtual void SetGameCompletedEventCallback(
            const MessageCallback<GameCompletedEventData>& callback
        ) = 0;
    };
}
