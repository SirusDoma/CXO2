#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Models/Room.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <string>

namespace Cx
{
    class StateWaiting7K;

    struct WaitingMemberJoinedEventData;
    struct WaitingMemberLeftEventData;
    struct WaitingMemberTeamChangedEventData;
    struct WaitingMemberReadyStateChangedEventData;
    struct WaitingSlotChangedEventData;
    struct WaitingKickEventData;
    struct WaitingMusicChangedEventData;
    struct WaitingTitleChangedEventData;
    struct WaitingMapChangedEventData;
    struct StartGameEventData;
    struct ExitWaitingResponse;

    enum class WaitingEventKey
    {
        OnStartGameCheck,
        OnReady,
        OnTeamChange,
        OnSlotChange,
        OnEmoticon,
        OnMemberJoined,
        OnMemberLeft,
        OnMemberTeamChanged,
        OnMemberReadyStateChanged,
        OnSlotChanged,
        OnKicked,
        OnMusicChanged,
        OnTitleChanged,
        OnMapChanged,
        OnStartGame,
        OnExitRoomResponded
    };

    struct WaitingEventArgs : Interceptable {};

    struct WaitingStartGameEventArgs : WaitingEventArgs
    {
        bool Authorized = false;
    };

    struct WaitingReadyEventArgs : Interceptable
    {
        const bool Ready;

        explicit WaitingReadyEventArgs(const bool ready) : Ready(ready) {}
    };

    struct WaitingTeamEventArgs : Interceptable
    {
        Room::Team& Team;

        explicit WaitingTeamEventArgs(Room::Team& team) : Team(team) {}
    };

    struct WaitingSlotEventArgs : Interceptable
    {
        unsigned int& Slot;
        const bool Kick;

        WaitingSlotEventArgs(unsigned int& slot, const bool kick) :
            Slot(slot),
            Kick(kick)
        {
        }
    };

    struct WaitingEmoticonEventArgs : Interceptable
    {
        std::string& Command;

        explicit WaitingEmoticonEventArgs(std::string& command) : Command(command) {}
    };

    struct WaitingMemberJoinedEventArgs : Interceptable
    {
        const WaitingMemberJoinedEventData& Data;

        explicit WaitingMemberJoinedEventArgs(const WaitingMemberJoinedEventData& data) : Data(data) {}
    };

    struct WaitingMemberLeftEventArgs : Interceptable
    {
        const WaitingMemberLeftEventData& Data;

        explicit WaitingMemberLeftEventArgs(const WaitingMemberLeftEventData& data) : Data(data) {}
    };

    struct WaitingMemberTeamChangedEventArgs : Interceptable
    {
        const WaitingMemberTeamChangedEventData& Data;

        explicit WaitingMemberTeamChangedEventArgs(const WaitingMemberTeamChangedEventData& data) : Data(data) {}
    };

    struct WaitingMemberReadyStateChangedEventArgs : Interceptable
    {
        const WaitingMemberReadyStateChangedEventData& Data;

        explicit WaitingMemberReadyStateChangedEventArgs(const WaitingMemberReadyStateChangedEventData& data) : Data(data) {}
    };

    struct WaitingSlotChangedEventArgs : Interceptable
    {
        const WaitingSlotChangedEventData& Data;

        explicit WaitingSlotChangedEventArgs(const WaitingSlotChangedEventData& data) : Data(data) {}
    };

    struct WaitingKickedEventArgs : Interceptable
    {
        const WaitingKickEventData& Data;

        explicit WaitingKickedEventArgs(const WaitingKickEventData& data) : Data(data) {}
    };

    struct WaitingMusicChangedEventArgs : Interceptable
    {
        const WaitingMusicChangedEventData& Data;

        explicit WaitingMusicChangedEventArgs(const WaitingMusicChangedEventData& data) : Data(data) {}
    };

    struct WaitingTitleChangedEventArgs : Interceptable
    {
        const WaitingTitleChangedEventData& Data;

        explicit WaitingTitleChangedEventArgs(const WaitingTitleChangedEventData& data) : Data(data) {}
    };

    struct WaitingMapChangedEventArgs : Interceptable
    {
        const WaitingMapChangedEventData& Data;

        explicit WaitingMapChangedEventArgs(const WaitingMapChangedEventData& data) : Data(data) {}
    };

    struct WaitingGameStartedEventArgs : Interceptable
    {
        const StartGameEventData& Data;

        explicit WaitingGameStartedEventArgs(const StartGameEventData& data) : Data(data) {}
    };

    struct WaitingExitRoomEventArgs : Interceptable
    {
        const ExitWaitingResponse& Response;

        explicit WaitingExitRoomEventArgs(const ExitWaitingResponse& response) : Response(response) {}
    };

    using WaitingStartGameCheckEvent          = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingStartGameEventArgs>;
    using WaitingReadyEvent                   = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingReadyEventArgs>;
    using WaitingTeamEvent                    = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingTeamEventArgs>;
    using WaitingSlotEvent                    = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingSlotEventArgs>;
    using WaitingEmoticonEvent                = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingEmoticonEventArgs>;
    using WaitingMemberJoinedEvent            = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMemberJoinedEventArgs>;
    using WaitingMemberLeftEvent              = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMemberLeftEventArgs>;
    using WaitingMemberTeamChangedEvent       = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMemberTeamChangedEventArgs>;
    using WaitingMemberReadyStateChangedEvent = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMemberReadyStateChangedEventArgs>;
    using WaitingSlotChangedEvent             = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingSlotChangedEventArgs>;
    using WaitingKickedEvent                  = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingKickedEventArgs>;
    using WaitingMusicChangedEvent            = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMusicChangedEventArgs>;
    using WaitingTitleChangedEvent            = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingTitleChangedEventArgs>;
    using WaitingMapChangedEvent              = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingMapChangedEventArgs>;
    using WaitingStartGameEvent               = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingGameStartedEventArgs>;
    using WaitingExitRoomEvent                = Gx::Event<WaitingEventKey, StateWaiting7K, WaitingExitRoomEventArgs>;

    namespace WaitingEvents
    {
        inline constexpr WaitingStartGameCheckEvent          OnStartGameCheck{WaitingEventKey::OnStartGameCheck};
        inline constexpr WaitingReadyEvent                   OnReady{WaitingEventKey::OnReady};
        inline constexpr WaitingTeamEvent                    OnTeamChange{WaitingEventKey::OnTeamChange};
        inline constexpr WaitingSlotEvent                    OnSlotChange{WaitingEventKey::OnSlotChange};
        inline constexpr WaitingEmoticonEvent                OnEmoticon{WaitingEventKey::OnEmoticon};
        inline constexpr WaitingMemberJoinedEvent            OnMemberJoined{WaitingEventKey::OnMemberJoined};
        inline constexpr WaitingMemberLeftEvent              OnMemberLeft{WaitingEventKey::OnMemberLeft};
        inline constexpr WaitingMemberTeamChangedEvent       OnMemberTeamChanged{WaitingEventKey::OnMemberTeamChanged};
        inline constexpr WaitingMemberReadyStateChangedEvent OnMemberReadyStateChanged{WaitingEventKey::OnMemberReadyStateChanged};
        inline constexpr WaitingSlotChangedEvent             OnSlotChanged{WaitingEventKey::OnSlotChanged};
        inline constexpr WaitingKickedEvent                  OnKicked{WaitingEventKey::OnKicked};
        inline constexpr WaitingMusicChangedEvent            OnMusicChanged{WaitingEventKey::OnMusicChanged};
        inline constexpr WaitingTitleChangedEvent            OnTitleChanged{WaitingEventKey::OnTitleChanged};
        inline constexpr WaitingMapChangedEvent              OnMapChanged{WaitingEventKey::OnMapChanged};
        inline constexpr WaitingStartGameEvent               OnStartGame{WaitingEventKey::OnStartGame};
        inline constexpr WaitingExitRoomEvent                OnExitRoomResponded{WaitingEventKey::OnExitRoomResponded};
    }
}
