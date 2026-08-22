#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>
#include <CXO2/Models/Shop.hpp>
#include <CXO2/Services/MusicDownloaderService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <CXO2/UI/Image.hpp>

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

        void OnShowAllButtonClicked(Control& sender, Control::Event& ev);
        void OnShowBuyableButtonClicked(Control& sender, Control::Event& ev);

        void OnDownloadTabButtonClicked(Control& sender, Control::Event& ev);
        void OnCartTabButtonClicked(Control& sender, Control::Event& ev);

        void OnMusicItemFocusChanged(Control& sender, Control::Event& ev);
        void OnMusicItemToggleClicked(Control& sender, Control::Event& ev);
        void OnMusicItemToggleFocusChanged(Control& sender, Control::Event& ev);
        void OnMusicItemClicked(Control& sender, Control::Event& ev);

        void OnMusicPrevPageButtonClicked(Control& sender, Control::Event& ev);
        void OnMusicNextPageButtonClicked(Control& sender, Control::Event& ev);
        void OnMusicListScrolled(Control& sender, Control::Event& ev);

        void OnShopPrevPageButtonClicked(Control& sender, Control::Event& ev);
        void OnShopNextPageButtonClicked(Control& sender, Control::Event& ev);
        void OnShopListScrolled(Control& sender, Control::Event& ev);

        void OnShopSortButtonClicked(Control& sender, Control::Event& ev);
        void OnMusicSortButtonClicked(Control& sender, Control::Event& ev);

        void OnDownloadButtonClicked(Control& sender, Control::Event& ev);
        void OnCancelButtonClicked(Control& sender, Control::Event& ev);
        void OnCancelDialogAnswered(bool answer);

        void OnDownloadStarted(std::uint16_t musicID);
        void OnDownloadRenaming(std::uint16_t musicID);
        void OnDownloadProgressed(const MusicDownloadProgress& progress);
        void OnQueueCompleted();
        void OnDownloadFailed(MusicDownloadError error);

        void OnBuyButtonClicked(Control& sender, Control::Event& ev, const ChartMetadata& entry);
        void OnCartBuyButtonClicked(Control& sender, Control::Event& ev);
        void OnCartBuyDialogAnswered(bool answer);
        void OnCartGiftButtonClicked(Control& sender, Control::Event& ev);

        void OnCartPrevPageButtonClicked(Control& sender, Control::Event& ev);
        void OnCartNextPageButtonClicked(Control& sender, Control::Event& ev);
        void OnCartListScrolled(Control& sender, Control::Event& ev);
        void OnCartItemDeleteButtonClicked(Control& sender, Control::Event& ev);

        void OnBackButtonClicked(Control& sender, Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        CartContext& m_cart;
        ItemFactory& m_items;
        MusicDownloaderService& m_downloader;

        unsigned int m_cartCurrentPage;
        std::unordered_map<Control*, std::size_t> m_cartDeleteButtonIndices;

        unsigned int m_musicCurrentPage;
        std::vector<ChartMetadata> m_musicList;
        std::unordered_map<Control*, std::size_t> m_musicItemIndices;

        unsigned int m_shopCurrentPage;
        std::vector<ChartMetadata> m_shopList;

        std::unordered_map<Control*, MusicSortKey> m_sortKeys;
        MusicSortKey m_shopSortKey{MusicSortKey::Title};
        MusicSortKey m_musicSortKey{MusicSortKey::Title};
        bool m_shopSortAscending  = true;
        bool m_musicSortAscending = true;

        Image* m_selector;
    };
}
