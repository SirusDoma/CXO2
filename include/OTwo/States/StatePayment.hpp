#pragma once

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class SessionContext;
class ItemFactory;
class CartContext;
class StatePayment : public State
{
public:
    explicit StatePayment(Gx::AudioMixer& mixer, SessionContext& session, ItemFactory& items, CartContext& cart);
    void Initialize() override;

private:
    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;
    CartContext& m_cart;
};
