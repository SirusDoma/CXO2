#ifndef O2JAM_ROOM_BUTTON_HPP
#define O2JAM_ROOM_BUTTON_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>

#include <OTwo/Models/Room.hpp>

class RoomButton : public Gx::Image
{
public:
    RoomButton();

    void Initialize() override;
    sf::FloatRect GetLocalBounds() const override;

    bool IsActive() const;

    const Room &GetRoomInfo() const;
    void SetRoomInfo(const Room& data);
    void Reset();

private:
    void OnMouseMove(sf::Event::MouseMoveEvent ev) override;
    void Invalidate() override;

    Room m_room;
    Gx::Image *m_hover;
    bool m_active;
};

#endif
