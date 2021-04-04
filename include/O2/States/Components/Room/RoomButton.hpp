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
    RoomButton(Gx::Scene &scene);

    virtual const sf::FloatRect GetLocalBounds() const;

    const RoomData GetRoomData() const;
    void SetRoomData(const RoomData& data);

private:
    virtual void Invalidate();

    Gx::Scene *m_scene;
    Gx::Button *m_button;
    Gx::Label  *m_titleLabel, *m_musicLabel;
    Gx::Number *m_numberLabel;
    Gx::Image  *m_speedLabel, *m_gameMode, *m_stateLabel, *m_ohmLevel, *m_lock;

    RoomData m_data;
};

#endif
