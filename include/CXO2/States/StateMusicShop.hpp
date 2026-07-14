#pragma once

#include <CXO2/States/State.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/Image.hpp>

#include <unordered_map>

namespace Cx
{
    class CartContext;
    class SessionContext;
    class ItemFactory;
    class StateMusicShop : public State
    {
    public:
        StateMusicShop(Gx::AudioMixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items);
        void Initialize() override;

    private:
        void SelectMusicFilter(bool showAll);
        void SelectShopTab(bool download);
        void InvalidateCart();

        void OnShowAllButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShowBuyableButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnDownloadTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMusicItemFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemToggleClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemToggleFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicItemClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnBuyButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnBuyDialogAnswered(bool answer);
        void OnGiftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnCartPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartListScrolled(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartItemDeleteButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        CartContext& m_cart;
        ItemFactory& m_items;

        unsigned int m_cartCurrentPage;
        std::unordered_map<Gx::Control*, std::size_t> m_cartDeleteButtonIndices;

        Gx::Image* m_selector;
    };
}
