#ifndef O2JAM_STATE_MY_ROOM_HPP
#define O2JAM_STATE_MY_ROOM_HPP


#include <Genode/UI/Image.hpp>
#include <OTwo/States/State.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Room.hpp>

class StateMyRoom : public State
{
public:
    StateMyRoom();

    void Initialize() override;

private:
    void Invalidate();
    void InvalidateSlot(Gx::Image* slot, EquipmentType type, RenderPart preview = RenderPart::LargePreview);

    unsigned int m_bagCurrentPage = 0;
    unsigned int m_bagMaxPage     = 0;

    Item* m_selectedItem;
    Gx::Image* m_bagSelect;
    std::vector<Item*> m_inventory;
};
#endif
