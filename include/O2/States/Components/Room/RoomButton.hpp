#ifndef O2JAM_ROOM_ROOMBUTTON_HPP
#define O2JAM_ROOM_ROOMBUTTON_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>

#include <O2/Data/Room.hpp>

class RoomButton : public Gx::Control
{
public:
    RoomButton();

    void Initialize(Gx::Scene &scene);
    virtual const sf::FloatRect GetLocalBounds() const;

    bool IsActive() const;

    const Room::RoomData GetRoomData() const;
    void SetRoomData(const Room::RoomData& data);
    void Deactivate();

private:
    virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
    virtual void Invalidate();

    Gx::Button *m_button;
    Gx::Label  *m_titleLabel, *m_musicLabel, *m_capacityLabel;
    Gx::Number *m_numberLabel;
    Gx::Image  *m_hover, *m_speedLabel, *m_gameMode, *m_stateLabel, *m_ohmLevel, *m_lock;

    Room::RoomData m_data;
    bool m_active;
};

#endif
