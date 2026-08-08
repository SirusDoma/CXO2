#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>
#include <CXO2/Models/Shop.hpp>
#include <CXO2/Services/MusicDownloaderService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/Image.hpp>

#include <optional>
#include <unordered_map>
#include <vector>

namespace Cx
{
    class CartContext;
    class SessionContext;
    class ItemFactory;
    class StateMusicShop : public State
    {
    public:
        StateMusicShop(Gx::AudioMixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items, MusicDownloaderService& downloader);
        void Initialize() override;

        void OnDownloadCompleted(std::uint16_t musicID);

    protected:
        void Finalize() override;

    private:
        void SelectMusicFilter(bool showAll);
        void SelectShopTab(bool download);
        void InvalidateCart();
        void InvalidateMusicList();
        void InvalidateShopList();
        void InvalidateDownloadPanel();
        void SetDownloadStatus(const sf::String& status);
        void SortMusicList(std::vector<ChartMetadata>& list, MusicSortKey key, bool ascending) const;

        void OnShowAllButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShowBuyableButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnDownloadTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMusicItemFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemToggleClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemToggleFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMusicPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicListScrolled(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShopPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopListScrolled(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShopSortButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicSortButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnDownloadButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCancelButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCancelDialogAnswered(bool answer);

        void OnDownloadStarted(std::uint16_t musicID);
        void OnDownloadRenaming(std::uint16_t musicID);
        void OnDownloadProgressed(const MusicDownloadProgress& progress);
        void OnQueueCompleted();
        void OnDownloadFailed(MusicDownloadError error);

        void OnBuyButtonClicked(Gx::Control& sender, Gx::Control::Event& ev, const ChartMetadata& entry);
        void OnCartBuyButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartBuyDialogAnswered(bool answer);
        void OnCartGiftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnCartPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartListScrolled(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartItemDeleteButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        CartContext& m_cart;
        ItemFactory& m_items;
        MusicDownloaderService& m_downloader;

        unsigned int m_cartCurrentPage;
        std::unordered_map<Gx::Control*, std::size_t> m_cartDeleteButtonIndices;

        unsigned int m_musicCurrentPage;
        std::vector<ChartMetadata> m_musicList;
        std::unordered_map<Gx::Control*, std::size_t> m_musicItemIndices;

        unsigned int m_shopCurrentPage;
        std::vector<ChartMetadata> m_shopList;

        std::unordered_map<Gx::Control*, MusicSortKey> m_sortKeys;
        MusicSortKey m_shopSortKey{MusicSortKey::Title};
        MusicSortKey m_musicSortKey{MusicSortKey::Title};
        bool m_shopSortAscending  = true;
        bool m_musicSortAscending = true;

        Gx::Image* m_selector;
    };
}
