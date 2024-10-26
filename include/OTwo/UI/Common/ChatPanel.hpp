#pragma once

#include <Genode/UI/UiContainer.hpp>
#include <OTwo/UI/Common/ChatWindow.hpp>
#include <Genode/UI/TextBox.hpp>

class SessionContext;
class ChatPanel : public Gx::UiContainer
{
public:
    explicit ChatPanel(SessionContext& session);
    void Initialize() override;

    void SetInputEnabled(bool enabled);
    void SetMaximumTextLength(unsigned int length);

    ChatWindow* GetChatWindow() const;

private:
    SessionContext& m_session;
};
