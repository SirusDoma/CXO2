#pragma once

#include <OTwo/UI/Common/ChatWindow.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/InputField.hpp>

class SessionContext;
class MessagingService;
class ChatPanel : public Gx::UiContainer
{
public:
    explicit ChatPanel(SessionContext& session, MessagingService& service);

    void Initialize() override;
    void Finalize() override;

    void SetInputEnabled(bool enabled);
    void SetMaximumTextLength(unsigned int length);

    ChatWindow* GetChatWindow() const;

private:
    sf::String m_recipient{};
    SessionContext& m_session;
    MessagingService& m_service;
};
