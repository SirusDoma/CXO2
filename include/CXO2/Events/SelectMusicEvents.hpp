#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace Cx
{
    class SelectMusicDialog;

    enum class SelectMusicEventKey
    {
        OnInitialize,
        OnPresent,
        OnDismiss,
        OnInvalidate,
        OnSort,
        OnRandomToggle,
        OnChangePage
    };

    struct SelectMusicEventArgs : Interceptable {};

    struct SelectMusicDismissEventArgs : Interceptable
    {
        bool Accepted;

        explicit SelectMusicDismissEventArgs(const bool accepted) : Accepted(accepted) {}
    };

    struct SelectMusicSortEventArgs : Interceptable
    {
        MusicSortMode& Mode;

        explicit SelectMusicSortEventArgs(MusicSortMode& mode) : Mode(mode) {}
    };

    struct SelectMusicRandomEventArgs : Interceptable
    {
        LevelCategory& Level;
        bool Checked;

        SelectMusicRandomEventArgs(LevelCategory& level, const bool checked) : Level(level), Checked(checked) {}
    };

    struct SelectMusicPageEventArgs : Interceptable
    {
        unsigned int& Page;

        explicit SelectMusicPageEventArgs(unsigned int& page) : Page(page) {}
    };

    using SelectMusicEvent        = Gx::Event<SelectMusicEventKey, SelectMusicDialog, SelectMusicEventArgs>;
    using SelectMusicDismissEvent = Gx::Event<SelectMusicEventKey, SelectMusicDialog, SelectMusicDismissEventArgs>;
    using SelectMusicSortEvent    = Gx::Event<SelectMusicEventKey, SelectMusicDialog, SelectMusicSortEventArgs>;
    using SelectMusicRandomEvent  = Gx::Event<SelectMusicEventKey, SelectMusicDialog, SelectMusicRandomEventArgs>;
    using SelectMusicPageEvent    = Gx::Event<SelectMusicEventKey, SelectMusicDialog, SelectMusicPageEventArgs>;

    namespace SelectMusicEvents
    {
        inline constexpr SelectMusicEvent        OnInitialize{SelectMusicEventKey::OnInitialize};
        inline constexpr SelectMusicEvent        OnPresent{SelectMusicEventKey::OnPresent};
        inline constexpr SelectMusicDismissEvent OnDismiss{SelectMusicEventKey::OnDismiss};
        inline constexpr SelectMusicEvent        OnInvalidate{SelectMusicEventKey::OnInvalidate};
        inline constexpr SelectMusicSortEvent    OnSort{SelectMusicEventKey::OnSort};
        inline constexpr SelectMusicRandomEvent  OnRandomToggle{SelectMusicEventKey::OnRandomToggle};
        inline constexpr SelectMusicPageEvent    OnChangePage{SelectMusicEventKey::OnChangePage};
    }
}
