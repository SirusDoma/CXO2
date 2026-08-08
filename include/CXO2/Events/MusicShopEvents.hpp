#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>
#include <CXO2/Models/Shop.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <cstdint>

namespace Cx
{
    class StateMusicShop;

    enum class MusicShopEventKey
    {
        OnDownloadInitializing,
        OnMusicPurchase,
        OnMusicShopListSort,
        OnAcquiredMusicListSort,
        OnMusicShopListInvalidate,
        OnAcquiredMusicListInvalidate,
        OnDownloadPanelInvalidate,
        OnDownloadStarted,
        OnDownloadRenaming,
        OnDownloadCompleted,
        OnDownloadProgress,
        OnDownloadCancel,
        OnDownloadFailed,
        OnQueueCompleted
    };

    struct MusicShopEventArgs : Interceptable {};

    struct MusicShopPurchaseEventArgs : Interceptable
    {
        ChartMetadata& Music;

        explicit MusicShopPurchaseEventArgs(ChartMetadata& music) : Music(music) {}
    };

    struct MusicShopSortEventArgs : Interceptable
    {
        MusicSortKey& Key;
        bool& Ascending;

        MusicShopSortEventArgs(MusicSortKey& key, bool& ascending) :
            Key(key),
            Ascending(ascending)
        {
        }
    };

    struct MusicShopDownloadEventArgs : Interceptable
    {
        const std::uint16_t MusicID;

        explicit MusicShopDownloadEventArgs(const std::uint16_t musicID) : MusicID(musicID) {}
    };

    struct MusicShopDownloadErrorEventArgs : Interceptable
    {
        const MusicDownloadError Error;

        explicit MusicShopDownloadErrorEventArgs(const MusicDownloadError error) : Error(error) {}
    };

    struct MusicShopDownloadProgressEventArgs : Interceptable
    {
        const MusicDownloadProgress& Progress;

        explicit MusicShopDownloadProgressEventArgs(const MusicDownloadProgress& progress) : Progress(progress) {}
    };

    using MusicShopEvent                 = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopEventArgs>;
    using MusicShopPurchaseEvent         = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopPurchaseEventArgs>;
    using MusicShopSortEvent             = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopSortEventArgs>;
    using MusicShopDownloadEvent         = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopDownloadEventArgs>;
    using MusicShopDownloadErrorEvent    = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopDownloadErrorEventArgs>;
    using MusicShopDownloadProgressEvent = Gx::Event<MusicShopEventKey, StateMusicShop, MusicShopDownloadProgressEventArgs>;

    namespace MusicShopEvents
    {
        inline constexpr MusicShopPurchaseEvent         OnMusicPurchase{MusicShopEventKey::OnMusicPurchase};
        inline constexpr MusicShopSortEvent             OnMusicShopListSort{MusicShopEventKey::OnMusicShopListSort};
        inline constexpr MusicShopSortEvent             OnAcquiredMusicListSort{MusicShopEventKey::OnAcquiredMusicListSort};
        inline constexpr MusicShopEvent                 OnMusicShopListInvalidate{MusicShopEventKey::OnMusicShopListInvalidate};
        inline constexpr MusicShopEvent                 OnAcquiredMusicListInvalidate{MusicShopEventKey::OnAcquiredMusicListInvalidate};
        inline constexpr MusicShopEvent                 OnDownloadPanelInvalidate{MusicShopEventKey::OnDownloadPanelInvalidate};
        inline constexpr MusicShopEvent                 OnDownloadInitializing{MusicShopEventKey::OnDownloadInitializing};
        inline constexpr MusicShopDownloadEvent         OnDownloadStarted{MusicShopEventKey::OnDownloadStarted};
        inline constexpr MusicShopDownloadEvent         OnDownloadRenaming{MusicShopEventKey::OnDownloadRenaming};
        inline constexpr MusicShopDownloadEvent         OnDownloadCompleted{MusicShopEventKey::OnDownloadCompleted};
        inline constexpr MusicShopDownloadProgressEvent OnDownloadProgress{MusicShopEventKey::OnDownloadProgress};
        inline constexpr MusicShopEvent                 OnDownloadCancel{MusicShopEventKey::OnDownloadCancel};
        inline constexpr MusicShopDownloadErrorEvent    OnDownloadFailed{MusicShopEventKey::OnDownloadFailed};
        inline constexpr MusicShopEvent                 OnQueueCompleted{MusicShopEventKey::OnQueueCompleted};
    }
}
