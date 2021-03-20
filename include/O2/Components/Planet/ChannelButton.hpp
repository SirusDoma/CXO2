#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Image.hpp>

class ChannelButton : Gx::Button
{
public:
    ChannelButton();

protected:
    virtual void Invalidate();

private:
    void Initialize();

    Gx::Image *m_selection;
};

#endif
