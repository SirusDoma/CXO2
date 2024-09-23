#ifndef O2JAM_STATE_MY_ROOM_HPP
#define O2JAM_STATE_MY_ROOM_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Room.hpp>

#include <Genode/UI/Image.hpp>

class SessionContext;
class ItemFactory;
class StateMyRoom : public State
{
public:
    StateMyRoom(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items);

    void Initialize() override;

private:
    void Invalidate();
    void InvalidateSlot(Gx::Image* slot, EquipmentType type, RenderPart thumbnail = RenderPart::LargeThumbnail);

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;

    unsigned int m_bagCurrentPage = 0;
    unsigned int m_bagMaxPage     = 0;

    Item* m_selectedItem;
    Gx::Image* m_bagSelectIndicator;
    std::vector<Item*> m_inventory;
};
#endif
