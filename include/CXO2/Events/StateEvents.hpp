#pragma once

#include <CXO2/Events/Interceptable.hpp>

#include <Genode/Events/EventDispatcher.hpp>
#include <Genode/Graphics/RenderStates.hpp>
#include <Genode/Graphics/RenderSurface.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <cstdint>
#include <string_view>

namespace Cx
{
    class State;
    class GameContext;

    enum class RoomTransitionEventType : std::uint8_t;

    enum class StateEventKey
    {
        OnInitialize,
        OnExtend,
        OnFinalize,
        OnUpdate,
        OnRender,
        OnInput
    };

    namespace StateKey
    {
        constexpr static auto Avi       = "STATE_AVI";
        constexpr static auto Planet    = "STATE_PLANET";
        constexpr static auto Room      = "STATE_ROOM";
        constexpr static auto MyRoom    = "STATE_MY_ROOM";
        constexpr static auto ItemShop  = "STATE_ITEM_SHOP";
        constexpr static auto MusicShop = "STATE_MUSIC_SHOP";
        constexpr static auto Bulletin  = "STATE_BULLETIN";
        constexpr static auto Payment   = "STATE_PAYMENT";
        constexpr static auto Waiting7K = "STATE_WAITING_7K";
        constexpr static auto Loading   = "STATE_LOADING";
        constexpr static auto Playing7K = "STATE_PLAYING_7K";
        constexpr static auto Result    = "STATE_RESULT";
    }

    enum class StateEventArgsType
    {
        Default,
        Game,
        Room
    };

    struct StateEventArgs : Interceptable
    {
        const std::string_view Key;
        const StateEventArgsType Type;

        explicit StateEventArgs(const std::string_view key) :
            Key(key),
            Type(StateEventArgsType::Default)
        {
        }

    protected:
        StateEventArgs(const std::string_view key, const StateEventArgsType type) :
            Key(key),
            Type(type)
        {
        }
    };

    struct StateGameEventArgs : StateEventArgs
    {
        GameContext& Context;

        StateGameEventArgs(const std::string_view key, GameContext& context) :
            StateEventArgs(key, StateEventArgsType::Game),
            Context(context)
        {
        }
    };

    struct StateRoomEventArgs : StateEventArgs
    {
        RoomTransitionEventType& Transition;

        StateRoomEventArgs(const std::string_view key, RoomTransitionEventType& transition) :
            StateEventArgs(key, StateEventArgsType::Room),
            Transition(transition)
        {
        }
    };

    struct StateUpdateEventArgs : Interceptable { sf::Time Delta; };
    struct StateRenderEventArgs : Interceptable { Gx::RenderSurface& Surface; Gx::RenderStates& States; };
    struct StateInputEventArgs  : Interceptable { sf::Event Input; };

    using StateEvent       = Gx::Event<StateEventKey, State, StateEventArgs>;
    using StateUpdateEvent = Gx::Event<StateEventKey, State, StateUpdateEventArgs>;
    using StateRenderEvent = Gx::Event<StateEventKey, State, StateRenderEventArgs>;
    using StateInputEvent  = Gx::Event<StateEventKey, State, StateInputEventArgs>;

    namespace StateEvents
    {
        inline constexpr StateEvent       OnInitialize{StateEventKey::OnInitialize};
        inline constexpr StateEvent       OnExtend{StateEventKey::OnExtend};
        inline constexpr StateEvent       OnFinalize{StateEventKey::OnFinalize};
        inline constexpr StateUpdateEvent OnUpdate{StateEventKey::OnUpdate};
        inline constexpr StateRenderEvent OnRender{StateEventKey::OnRender};
        inline constexpr StateInputEvent  OnInput{StateEventKey::OnInput};
    }
}
