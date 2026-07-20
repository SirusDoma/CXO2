#pragma once

#include <CXO2/UI/Common/ChatWindow.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/WhisperMessageResponse.hpp>
#include <CXO2/Network/Responses/MainRoomUserMessageResponse.hpp>
#include <CXO2/Network/Responses/MainRoomAdminMessageResponse.hpp>
#include <CXO2/Network/Responses/WaitingUserMessageResponse.hpp>
#include <CXO2/Network/Responses/WaitingAdminMessageResponse.hpp>

#include <CXO2/Network/Events/WhisperEventData.hpp>

#include <string>
#include <vector>

namespace Cx
{
    class SessionContext;
    class ChatService;
    class ChatPanel : public Gx::UiContainer
    {
    public:
        ChatPanel(SessionContext& session, ChatService& service);

        void Initialize() override;

        void SetInputEnabled(bool enabled);
        void SetMaximumTextLength(unsigned int length);

        ChatWindow* GetChatWindow() const;

    private:
        void OnMessage(const sf::String& sender, Role senderRole, const sf::String& text);

        void OnWhisper(const MessageEnvelope<WhisperEventData>& ev);
        void OnWaitingUserMessage(const MessageEnvelope<WaitingUserMessageResponse>& ev);
        void OnWaitingAdminMessage(const MessageEnvelope<WaitingAdminMessageResponse>& ev);
        void OnMainRoomUserMessage(const MessageEnvelope<MainRoomUserMessageResponse>& ev);
        void OnMainRoomAdminMessage(const MessageEnvelope<MainRoomAdminMessageResponse>& ev);

        void OnSendWhisperResponded(const MessageEnvelope<WhisperMessageResponse>& ev, const std::vector<std::string>& tokens, const sf::String& message);
        void OnSendWhisperResponded(const MessageEnvelope<WhisperMessageResponse>& ev, const sf::String& text);

        void OnChatScrollUpButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChatScrollDownButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnChatAllCheckChanged(Gx::RadioButton& radio, Gx::Control::Event& ev);
        void OnChatFallbackCheckChanged(Gx::RadioButton& radio);
        void OnChatWhisperCheckChanged(Gx::RadioButton& radio, Gx::Control::Event& ev);
        void OnWhisperDialogAccepted(Gx::InputField& nicknameInput, Gx::RadioButton& radio);

        void OnChatInputTextEntered(Gx::InputField& sender, const sf::String& text);

        sf::String m_recipient{};
        SessionContext& m_session;
        ChatService& m_service;
    };
}
