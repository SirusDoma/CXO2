#ifndef O2JAM_ROOM_CHAT_PANEL_HPP
#define O2JAM_ROOM_CHAT_PANEL_HPP

#include <Genode/UI/UiContainer.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>
#include <Genode/UI/TextBox.hpp>

class ChatPanel : public Gx::UiContainer
{
public:
    ChatPanel();
    void Initialize() override;

    void SetInputEnabled(bool enabled);
    void SetMaximumTextLength(unsigned int length);

    ChatWindow *GetChatWindow() const;
};

#endif
