#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Core/Chart.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Network/Requests/SubmitScoreRequest.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace Cx
{
    class StatePlaying7K;

    struct PlayingMemberStatsUpdateEventData;
    struct PlayingMemberScoreSubmissionEventData;
    struct PlayingMemberLeftEventData;
    struct GameCompletedEventData;

    enum class PlayingEventKey
    {
        OnInput,
        OnJudgement,
        OnJamCombo,
        OnSubmitScore,
        OnComplete,
        OnExit,
        OnMemberStatsUpdate,
        OnMemberScoreSubmitted,
        OnMemberLeft,
        OnGameCompleted
    };

    struct PlayingEventArgs : Interceptable {};

    struct PlayingInputEventArgs : Interceptable
    {
        const double RenderPosition;
        const Chart::Channel Channel;
        const bool State;

        PlayingInputEventArgs(const double renderPosition, const Chart::Channel channel, const bool state) :
            RenderPosition(renderPosition),
            Channel(channel),
            State(state)
        {
        }
    };

    struct PlayingJudgementEventArgs : Interceptable
    {
        const double RenderPosition;
        const Chart::NoteEvent& Note;
        const Cx::Accuracy Accuracy;
        const unsigned long long Count;

        PlayingJudgementEventArgs(const double renderPosition, const Chart::NoteEvent& note, const Cx::Accuracy accuracy, const unsigned long long count) :
            RenderPosition(renderPosition),
            Note(note),
            Accuracy(accuracy),
            Count(count)
        {
        }
    };

    struct PlayingJamComboEventArgs : Interceptable
    {
        const double RenderPosition;
        const Chart::NoteEvent& Note;
        const Cx::Accuracy Accuracy;
        const unsigned long long JamCombo;

        PlayingJamComboEventArgs(const double renderPosition, const Chart::NoteEvent& note, const Cx::Accuracy accuracy, const unsigned long long jamCombo) :
            RenderPosition(renderPosition),
            Note(note),
            Accuracy(accuracy),
            JamCombo(jamCombo)
        {
        }
    };

    struct PlayingSubmitScoreEventArgs : Interceptable
    {
        SubmitScoreRequest& Request;

        explicit PlayingSubmitScoreEventArgs(SubmitScoreRequest& request) : Request(request) {}
    };

    struct PlayingMemberStatsEventArgs : Interceptable
    {
        const PlayingMemberStatsUpdateEventData& Data;

        explicit PlayingMemberStatsEventArgs(const PlayingMemberStatsUpdateEventData& data) : Data(data) {}
    };

    struct PlayingMemberScoreEventArgs : Interceptable
    {
        const PlayingMemberScoreSubmissionEventData& Data;

        explicit PlayingMemberScoreEventArgs(const PlayingMemberScoreSubmissionEventData& data) : Data(data) {}
    };

    struct PlayingMemberLeftEventArgs : Interceptable
    {
        const PlayingMemberLeftEventData& Data;

        explicit PlayingMemberLeftEventArgs(const PlayingMemberLeftEventData& data) : Data(data) {}
    };

    struct PlayingGameCompletedEventArgs : Interceptable
    {
        const GameCompletedEventData& Data;

        explicit PlayingGameCompletedEventArgs(const GameCompletedEventData& data) : Data(data) {}
    };

    using PlayingEvent            = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingEventArgs>;
    using PlayingInputEvent       = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingInputEventArgs>;
    using PlayingJudgementEvent   = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingJudgementEventArgs>;
    using PlayingJamComboEvent    = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingJamComboEventArgs>;
    using PlayingSubmitScoreEvent   = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingSubmitScoreEventArgs>;
    using PlayingMemberStatsEvent   = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingMemberStatsEventArgs>;
    using PlayingMemberScoreEvent   = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingMemberScoreEventArgs>;
    using PlayingMemberLeftEvent    = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingMemberLeftEventArgs>;
    using PlayingGameCompletedEvent = Gx::Event<PlayingEventKey, StatePlaying7K, PlayingGameCompletedEventArgs>;

    namespace PlayingEvents
    {
        inline constexpr PlayingInputEvent         OnInput{PlayingEventKey::OnInput};
        inline constexpr PlayingJudgementEvent     OnJudgement{PlayingEventKey::OnJudgement};
        inline constexpr PlayingJamComboEvent      OnJamCombo{PlayingEventKey::OnJamCombo};
        inline constexpr PlayingSubmitScoreEvent   OnSubmitScore{PlayingEventKey::OnSubmitScore};
        inline constexpr PlayingEvent              OnComplete{PlayingEventKey::OnComplete};
        inline constexpr PlayingEvent              OnExit{PlayingEventKey::OnExit};
        inline constexpr PlayingMemberStatsEvent   OnMemberStatsUpdate{PlayingEventKey::OnMemberStatsUpdate};
        inline constexpr PlayingMemberScoreEvent   OnMemberScoreSubmitted{PlayingEventKey::OnMemberScoreSubmitted};
        inline constexpr PlayingMemberLeftEvent    OnMemberLeft{PlayingEventKey::OnMemberLeft};
        inline constexpr PlayingGameCompletedEvent OnGameCompleted{PlayingEventKey::OnGameCompleted};
    }
}
