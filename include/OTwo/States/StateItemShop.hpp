#ifndef O2JAM_STATE_ITEM_SHOP_HPP
#define O2JAM_STATE_ITEM_SHOP_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class SessionContext;
class ItemFactory;
class StateItemShop : public State
{
public:
    explicit StateItemShop(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items);
    void Initialize() override;

private:
    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;
};


#endif
