#pragma once

#include <CXO2/UI/Components/ChatWindow.hpp>

#include <CXO2/UI/UiContainer.hpp>
#include <CXO2/UI/InputField.hpp>
#include <CXO2/UI/RadioButton.hpp>

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
    class ChatPanel : public Cx::UiContainer
    {
    public:
        ChatPanel(SessionContext& session, ChatService& service);

        void Initialize() override;

        void SetInputEnabled(bool enabled);
        void SetMaximumTextLength(unsigned int length);

        ChatWindow* GetChatWindow() const;

    private:
        void ShowChatButtonToolTip(const Control& sender, const sf::String& message);

        void OnMessage(const sf::String& sender, Role senderRole, const sf::String& text);

        void OnWhisper(const MessageEnvelope<WhisperEventData>& ev);
        void OnWaitingUserMessage(const MessageEnvelope<WaitingUserMessageResponse>& ev);
        void OnWaitingAdminMessage(const MessageEnvelope<WaitingAdminMessageResponse>& ev);
        void OnMainRoomUserMessage(const MessageEnvelope<MainRoomUserMessageResponse>& ev);
        void OnMainRoomAdminMessage(const MessageEnvelope<MainRoomAdminMessageResponse>& ev);

        void OnSendWhisperResponded(const MessageEnvelope<WhisperMessageResponse>& ev);

        void OnChatScrollUpButtonClicked(Control& sender, Control::Event& ev);
        void OnChatScrollDownButtonClicked(Control& sender, Control::Event& ev);

        void OnChatAllFocusChanged(Control& sender, Control::Event& ev);
        void OnChatAllCheckChanged(Cx::RadioButton& radio, Control::Event& ev);

        void OnChatFriendFocusChanged(Control& sender, Control::Event& ev);
        void OnChatGuildFocusChanged(Control& sender, Control::Event& ev);
        void OnChatFallbackCheckChanged(Cx::RadioButton& radio);

        void OnChatWhisperFocusChanged(Control& sender, Control::Event& ev);
        void OnChatWhisperCheckChanged(Cx::RadioButton& radio, Control::Event& ev);

        void OnWhisperDialogAccepted(Cx::InputField& nicknameInput, Cx::RadioButton& radio);

        void OnChatInputTextEntered(Cx::InputField& sender, Cx::InputField::TextEnteredEvent& ev);

        sf::String m_recipient{};
        SessionContext& m_session;
        ChatService& m_service;
    };
}
