#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/UI/Planet/ChannelBoard.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <cstdint>

namespace Cx
{
    class StatePlanet;

    enum class AuthResult : std::uint32_t;

    struct ChannelListResponse;
    struct ChannelLoginResponse;

    enum class PlanetEventKey
    {
        OnPlanetEnter,
        OnChannelEnter,
        OnChannelBoardTabChange,
        OnAuthenticated,
        OnChannelListUpdated,
        OnChannelLogin
    };

    struct PlanetEnterEventArgs : Interceptable
    {
        MusicHall& Hall;

        explicit PlanetEnterEventArgs(MusicHall& hall) : Hall(hall) {}
    };

    struct ChannelEnterEventArgs : Interceptable
    {
        MusicHall& Hall;
        std::uint16_t& ServerID;
        std::uint16_t& ChannelID;

        ChannelEnterEventArgs(MusicHall& hall, std::uint16_t& serverID, std::uint16_t& channelID) :
            Hall(hall),
            ServerID(serverID),
            ChannelID(channelID)
        {
        }
    };

    struct PlanetTabEventArgs : Interceptable
    {
        ChannelBoard::Tab& Tab;

        explicit PlanetTabEventArgs(ChannelBoard::Tab& tab) : Tab(tab) {}
    };

    struct PlanetAuthEventArgs : Interceptable
    {
        const AuthResult Result;

        explicit PlanetAuthEventArgs(const AuthResult result) : Result(result) {}
    };

    struct PlanetChannelListEventArgs : Interceptable
    {
        const ChannelListResponse& Response;

        explicit PlanetChannelListEventArgs(const ChannelListResponse& response) : Response(response) {}
    };

    struct PlanetChannelLoginEventArgs : Interceptable
    {
        const ChannelLoginResponse& Response;

        explicit PlanetChannelLoginEventArgs(const ChannelLoginResponse& response) : Response(response) {}
    };

    using PlanetEnterEvent        = Gx::Event<PlanetEventKey, StatePlanet, PlanetEnterEventArgs>;
    using ChannelEnterEvent       = Gx::Event<PlanetEventKey, StatePlanet, ChannelEnterEventArgs>;
    using PlanetTabEvent          = Gx::Event<PlanetEventKey, StatePlanet, PlanetTabEventArgs>;
    using PlanetAuthEvent         = Gx::Event<PlanetEventKey, StatePlanet, PlanetAuthEventArgs>;
    using PlanetChannelListEvent  = Gx::Event<PlanetEventKey, StatePlanet, PlanetChannelListEventArgs>;
    using PlanetChannelLoginEvent = Gx::Event<PlanetEventKey, StatePlanet, PlanetChannelLoginEventArgs>;

    namespace PlanetEvents
    {
        inline constexpr PlanetEnterEvent        OnPlanetEnter{PlanetEventKey::OnPlanetEnter};
        inline constexpr ChannelEnterEvent       OnChannelEnter{PlanetEventKey::OnChannelEnter};
        inline constexpr PlanetTabEvent          OnChannelBoardTabChange{PlanetEventKey::OnChannelBoardTabChange};
        inline constexpr PlanetAuthEvent         OnAuthenticated{PlanetEventKey::OnAuthenticated};
        inline constexpr PlanetChannelListEvent  OnChannelListUpdated{PlanetEventKey::OnChannelListUpdated};
        inline constexpr PlanetChannelLoginEvent OnChannelLogin{PlanetEventKey::OnChannelLogin};
    }
}
