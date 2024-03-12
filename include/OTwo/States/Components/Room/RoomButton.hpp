#ifndef O2JAM_ROOM_ROOM_BUTTON_HPP
#define O2JAM_ROOM_ROOM_BUTTON_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>

#include <OTwo/Data/Room.hpp>

class RoomButton : public Gx::Image
{
public:
    RoomButton();

    void Initialize();
    virtual const sf::FloatRect GetLocalBounds() const;

    bool IsActive() const;

    const RoomData GetRoomData() const;
    void SetRoomData(const RoomData& data);
    void Reset();

private:
    virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
    virtual void Invalidate();

    RoomData m_room;
    Gx::Image *m_hover;
    bool m_active;
};

#endif
