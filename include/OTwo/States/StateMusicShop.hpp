#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/UI/Image.hpp>

class CartContext;
class SessionContext;
class ItemFactory;
class StateMusicShop : public State
{
public:
    StateMusicShop(Gx::AudioMixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items);
    void Initialize() override;

private:
    void OnBuyButtonClicked();
    void OnGiftButtonClicked();

    void InvalidateCart();

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    CartContext& m_cart;
    ItemFactory& m_items;

    unsigned int m_cartCurrentPage;

    Gx::Image* m_selector;
};
