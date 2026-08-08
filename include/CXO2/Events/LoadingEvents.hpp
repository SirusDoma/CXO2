#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Core/Chart.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace Cx
{
    class StateLoading;

    struct MemberMusicLoadedEventData;
    struct WaitingMemberLeftEventData;

    enum class LoadingEventKey
    {
        OnMusicLoaded,
        OnMemberMusicLoaded,
        OnMemberLeft
    };

    struct LoadingMusicEventArgs : Interceptable
    {
        const Cx::Chart& Chart;

        explicit LoadingMusicEventArgs(const Cx::Chart& chart) : Chart(chart) {}
    };

    struct LoadingMemberMusicLoadedEventArgs : Interceptable
    {
        const MemberMusicLoadedEventData& Data;

        explicit LoadingMemberMusicLoadedEventArgs(const MemberMusicLoadedEventData& data) : Data(data) {}
    };

    struct LoadingMemberLeftEventArgs : Interceptable
    {
        const WaitingMemberLeftEventData& Data;

        explicit LoadingMemberLeftEventArgs(const WaitingMemberLeftEventData& data) : Data(data) {}
    };

    using LoadingMusicEvent             = Gx::Event<LoadingEventKey, StateLoading, LoadingMusicEventArgs>;
    using LoadingMemberMusicLoadedEvent = Gx::Event<LoadingEventKey, StateLoading, LoadingMemberMusicLoadedEventArgs>;
    using LoadingMemberLeftEvent        = Gx::Event<LoadingEventKey, StateLoading, LoadingMemberLeftEventArgs>;

    namespace LoadingEvents
    {
        inline constexpr LoadingMusicEvent             OnMusicLoaded{LoadingEventKey::OnMusicLoaded};
        inline constexpr LoadingMemberMusicLoadedEvent OnMemberMusicLoaded{LoadingEventKey::OnMemberMusicLoaded};
        inline constexpr LoadingMemberLeftEvent        OnMemberLeft{LoadingEventKey::OnMemberLeft};
    }
}
