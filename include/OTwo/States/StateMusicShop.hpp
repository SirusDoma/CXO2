#ifndef O2JAM_STATE_MUSIC_SHOP_HPP
#define O2JAM_STATE_MUSIC_SHOP_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class CartContext;
class SessionContext;
class ItemFactory;
class StateMusicShop : public State
{
public:
    StateMusicShop(Gx::Mixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items);
    void Initialize() override;

private:
    void OnBuyButtonClicked();
    void OnGiftButtonClicked();

    void InvalidateCart();

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    CartContext& m_cart;
    ItemFactory& m_items;

    unsigned int m_cartCurrentPage;
};

#endif