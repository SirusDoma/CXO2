#ifndef O2JAM_ROOM_CHAT_PANEL_HPP
#define O2JAM_ROOM_CHAT_PANEL_HPP

#include <Genode/UI/UiContainer.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>

class ChatPanel : public Gx::UiContainer
{
public:
    ChatPanel();
    void Initialize();

    void SetInputEnabled(bool enabled);

    ChatWindow *GetChatWindow() const;
};

#endif
