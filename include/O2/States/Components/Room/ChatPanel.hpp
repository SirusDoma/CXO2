#ifndef O2JAM_ROOM_CHAT_PANEL_HPP
#define O2JAM_ROOM_CHAT_PANEL_HPP

#include <Genode/UI/UiContainer.hpp>

class ChatPanel : public Gx::UiContainer
{
public:
    ChatPanel();
    void Initialize(Gx::Scene &scene);
};

#endif
