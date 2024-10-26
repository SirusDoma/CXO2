#pragma once

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class SessionContext;
class CartContext;
class StatePayment : public State
{
public:
    explicit StatePayment(Gx::Mixer& mixer, SessionContext& session, CartContext& cart);
    void Initialize() override;

private:
    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    CartContext& m_cart;
};
