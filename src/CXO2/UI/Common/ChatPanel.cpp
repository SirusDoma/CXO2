#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Common/ChatWindow.hpp>

#include <CXO2/Events/ChatEvents.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Services/ChatService.hpp>

#include <CXO2/Utilities/StringFormatter.hpp>
#include <CXO2/Constants/Identifiers/ChatPanel.hpp>
#include <CXO2/Constants/Messages/Chat.hpp>
#include <CXO2/Constants/Messages/Room.hpp>

#include <cstring>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/InputField.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateWaiting7K.hpp>
#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/Models/Character.hpp>
#include <Genode/UI/ToolTip.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    ChatPanel::ChatPanel(SessionContext& session, ChatService& service, Gx::EventDispatcher& events) :
        Dispatchable(events),
        m_session(session),
        m_service(service)
    {
    }

    void ChatPanel::Initialize()
    {
        UiContainer::Initialize();

        if (Dispatch(ChatEvents::OnInitialize, ChatEventArgs{}))
            return;

        const auto chatWindow = GetChatWindow();

        m_service.SetWhisperEventCallback([this] (const auto& ev) { OnWhisper(ev); });

        if (GetParent<StateWaiting7K>() || GetParent<StatePlaying7K>())
        {
            m_service.SetWaitingUserMessageCallback([this] (const auto& ev) { OnWaitingUserMessage(ev); });
            m_service.SetWaitingAdminMessageCallback([this] (const auto& ev) { OnWaitingAdminMessage(ev); });
        }
        else
        {
            m_service.SetMainRoomUserMessageCallback([this] (const auto& ev) { OnMainRoomUserMessage(ev); });
            m_service.SetMainRoomAdminMessageCallback([this] (const auto& ev) { OnMainRoomAdminMessage(ev); });
        }

        if (const auto controls = FindChild<UiContainer>(Resource::ChatPanel::IDC_CONTAINER_CHAT_SCROLL_CONTROLS))
        {
            if (const auto scrollChat = controls->FindChild<Gx::ScrollBar>(Resource::ChatPanel::IDC_SCROLL_BAR_CHAT))
                chatWindow->SetScrollBar(*scrollChat);

            if (const auto btnChatScrollUp = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_UP))
                btnChatScrollUp->SetClickCallback([this] (auto& sender, auto& ev) { OnChatScrollUpButtonClicked(sender, ev); });

            if (const auto btnChatScrollDown = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_DOWN))
                btnChatScrollDown->SetClickCallback([this] (auto& sender, auto& ev) { OnChatScrollDownButtonClicked(sender, ev); });
        }

        if (const auto chatButtonList = FindChild<Gx::List>(Resource::ChatPanel::IDC_LIST_CHAT_BUTTON))
        {
            const auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_ALL);
            const auto btnChatFriend  = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_FRIEND);
            const auto btnChatGuild   = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_GUILD);
            const auto btnChatWhisper = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_WHISPER);

            btnChatAll->SetCheckedState(true);
            btnChatAll->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnChatAllFocusChanged(sender, ev); });
            btnChatAll->SetCheckStateChangeCallback([this] (auto& radio, auto& ev) { OnChatAllCheckChanged(radio, ev); });

            btnChatFriend->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnChatFriendFocusChanged(sender, ev); });
            btnChatFriend->SetCheckStateChangeCallback([this] (auto& radio, auto&) { OnChatFallbackCheckChanged(radio); });

            btnChatGuild->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnChatGuildFocusChanged(sender, ev); });
            btnChatGuild->SetCheckStateChangeCallback([this] (auto& radio, auto&) { OnChatFallbackCheckChanged(radio); });

            btnChatWhisper->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnChatWhisperFocusChanged(sender, ev); });
            btnChatWhisper->SetCheckStateChangeCallback([this] (auto& radio, auto& ev) { OnChatWhisperCheckChanged(radio, ev); });
        }

        const auto chatInput = FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT);
        chatInput->SetPermanentFocusEnabled(true);
        chatInput->SetTextEnteredCallback([this] (auto& sender, auto& ev) { OnChatInputTextEntered(sender, ev); });
    }

    void ChatPanel::OnMessage(const sf::String& sender, const Role senderRole, const sf::String& text)
    {
        auto args = ChatMessageEventArgs{senderRole, sender, text};
        if (Dispatch(ChatEvents::OnMessage, args))
            return;

        const auto chatWindow = GetChatWindow();

        if (Gx::StringHelper::StartsWith(args.Message.toAnsiString(), "/"))
        {
            if (const auto waiting = GetParent<StateWaiting7K>())
                waiting->OnMemberEmoticon(args.Sender, args.Message);
        }
        else
            chatWindow->PushMessage(args.Sender, senderRole, args.Message);
    }

    void ChatPanel::OnWhisper(const MessageEnvelope<WhisperEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            auto args = ChatWhisperEventArgs{response.Sender, response.Content};
            if (Dispatch(ChatEvents::OnWhisper, args))
                return;

            const auto chatWindow = GetChatWindow();
            chatWindow->PushWhisper(args.Sender, m_session.GetName(), args.Message);
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnWaitingUserMessage(const MessageEnvelope<WaitingUserMessageResponse>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            OnMessage(response.Sender, Role::Normal, response.Content);
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnWaitingAdminMessage(const MessageEnvelope<WaitingAdminMessageResponse>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            OnMessage(response.Sender, Role::Administrator, response.Content);
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnMainRoomUserMessage(const MessageEnvelope<MainRoomUserMessageResponse>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            OnMessage(response.Sender, Role::Normal, response.Content);
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnMainRoomAdminMessage(const MessageEnvelope<MainRoomAdminMessageResponse>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            OnMessage(response.Sender, Role::Administrator, response.Content);
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnSendWhisperResponded(const MessageEnvelope<WhisperMessageResponse>& ev)
    {
        const auto chatWindow = GetChatWindow();

        try
        {
            const auto& response = ev.Open();
            if (!response.Invalid)
            {
                chatWindow->PushWhisper(m_session.GetName(), response.Recipient, response.Content);
            }
            else
            {
                chatWindow->PushSystemMessage(fmt::format(Constants::Messages::Chat::WHISPER_NOT_DELIVERED, response.Recipient));
            }
        }
        catch (const Gx::Exception&)
        {
        }
    }

    void ChatPanel::OnChatScrollUpButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto controls   = FindChild<UiContainer>(Resource::ChatPanel::IDC_CONTAINER_CHAT_SCROLL_CONTROLS);
        const auto scrollChat = controls->FindChild<Gx::ScrollBar>(Resource::ChatPanel::IDC_SCROLL_BAR_CHAT);

        scrollChat->Decrease();
    }

    void ChatPanel::OnChatScrollDownButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto controls   = FindChild<UiContainer>(Resource::ChatPanel::IDC_CONTAINER_CHAT_SCROLL_CONTROLS);
        const auto scrollChat = controls->FindChild<Gx::ScrollBar>(Resource::ChatPanel::IDC_SCROLL_BAR_CHAT);

        scrollChat->Increase();
    }

    void ChatPanel::OnChatAllFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ShowChatButtonToolTip(sender, Constants::Messages::Chat::Tooltips::ALL);
    }

    void ChatPanel::OnChatAllCheckChanged(Gx::RadioButton& radio, Gx::Control::Event& ev)
    {
        if (radio.IsChecked())
            m_recipient = {};
    }

    void ChatPanel::OnChatFriendFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ShowChatButtonToolTip(sender, Constants::Messages::Chat::Tooltips::FRIEND);
    }

    void ChatPanel::OnChatGuildFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ShowChatButtonToolTip(sender, Constants::Messages::Chat::Tooltips::GUILD);
    }

    void ChatPanel::OnChatFallbackCheckChanged(Gx::RadioButton& radio)
    {
        const auto chatButtonList = FindChild<Gx::List>(Resource::ChatPanel::IDC_LIST_CHAT_BUTTON);
        const auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_ALL);

        if (radio.IsChecked())
            btnChatAll->SetCheckedState(true);
    }

    void ChatPanel::OnChatWhisperFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ShowChatButtonToolTip(sender, Constants::Messages::Chat::Tooltips::WHISPER);
    }

    void ChatPanel::OnChatWhisperCheckChanged(Gx::RadioButton& radio, Gx::Control::Event& ev)
    {
        const auto parent = GetParent<Cx::State>();
        if (!parent)
            return;

        if (radio.IsChecked())
        {
            if (const auto dialog = parent->Instantiate<Gx::Dialog>(Resource::ChatPanel::IDC_DIALOG_WHISPER); dialog)
            {
                if (const auto nicknameInput = dialog->FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_NICKNAME))
                {
                    nicknameInput->SetMaximumTextLength(20);
                    nicknameInput->SetString(m_recipient);
                    nicknameInput->SetFocus(true);
                    nicknameInput->SelectAll();

                    dialog->SetAcceptCallback([=, &radio] (auto&, auto&)
                    {
                        OnWhisperDialogAccepted(*nicknameInput, radio);
                    });

                    dialog->SetCancelCallback([=, &radio] (auto&, auto&)
                    {
                        OnChatFallbackCheckChanged(radio);
                    });

                    auto ctx   = Gx::DialogPresentationContext();
                    ctx.Bounds = {{}, parent->GetView().getSize()};
                    ctx.Prompt = Constants::Messages::Chat::WHISPER_TARGET_PROMPT;

                    parent->Present(*dialog, ctx);
                }
            }
        }
    }

    void ChatPanel::OnWhisperDialogAccepted(Gx::InputField& nicknameInput, Gx::RadioButton& radio)
    {
        if (nicknameInput.GetString().isEmpty())
            OnChatFallbackCheckChanged(radio);

        m_recipient = nicknameInput.GetString();
    }

    void ChatPanel::OnChatInputTextEntered(Gx::InputField& sender, Gx::InputField::TextEnteredEvent& ev)
    {
        const auto& text = ev.Text;
        const auto input = text.toAnsiString();

        if (Gx::StringHelper::StartsWith(input, Constants::Messages::Chat::Commands::EMOTICON))
        {
            if (GetParent<StateWaiting7K>())
                return;
        }

        if (Gx::StringHelper::StartsWith(input, Constants::Messages::Chat::Commands::ROOM_TITLE))
        {
            if (const auto waiting = GetParent<StateWaiting7K>())
                waiting->ChangeRoomTitle(Gx::StringHelper::Trim(text.substring(std::strlen(Constants::Messages::Chat::Commands::ROOM_TITLE))));
            else
                GetChatWindow()->PushSystemMessage(Constants::Messages::Room::TITLE_CHANGE_FORBIDDEN);

            return;
        }

        if (Gx::StringHelper::StartsWith(input, Constants::Messages::Chat::Commands::HELP_SHORT))
        {
            if (const auto waiting = GetParent<StateWaiting7K>())
                waiting->ShowChatHelp();
            else if (const auto room = GetParent<StateRoom>())
                room->ShowChatHelp();

            return;
        }

        if (Gx::StringHelper::StartsWith(text.toAnsiString(), Constants::Messages::Chat::Commands::WHISPER))
        {
            const auto tokens = Gx::StringHelper::Split(text.toAnsiString());
            if (tokens.size() < 3)
                return;

            m_recipient = tokens[1];
            if (const auto chatButtonList = FindChild<Gx::List>(Resource::ChatPanel::IDC_LIST_CHAT_BUTTON))
            {
                if (const auto btnChatWhisper = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_WHISPER))
                {
                    btnChatWhisper->SetEnabled(false);
                    btnChatWhisper->SetCheckedState(true);

                    btnChatWhisper->SetEnabled(true);
                }

                if (const auto parent = GetParent<Cx::State>())
                {
                    if (const auto dialog = parent->Instantiate<Gx::Dialog>(Resource::ChatPanel::IDC_DIALOG_WHISPER))
                    {
                        if (const auto nicknameInput = dialog->FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_NICKNAME))
                            nicknameInput->SetString(m_recipient);
                    }
                }
            }


            auto message   = Gx::StringHelper::Trim(text.substring(3 + tokens[1].size()));
            auto recipient = sf::String(tokens[1]);

            auto args = ChatSendEventArgs{ChatSendType::Whisper, message, recipient};
            if (Dispatch(ChatEvents::OnSend, args))
                return;

            m_service.SendWhisper(WhisperMessageRequest{args.Recipient, args.Message}, [=] (const MessageEnvelope<WhisperMessageResponse>& ev)
            {
                OnSendWhisperResponded(ev);
            });
        }
        else if (!m_recipient.isEmpty())
        {
            auto args = ChatSendEventArgs{ChatSendType::Whisper, text, m_recipient};
            if (Dispatch(ChatEvents::OnSend, args))
                return;

            m_service.SendWhisper(WhisperMessageRequest{args.Recipient, args.Message}, [=] (const MessageEnvelope<WhisperMessageResponse>& ev)
            {
                OnSendWhisperResponded(ev);
            });
        }
        else if (Gx::StringHelper::StartsWith(text.toAnsiString(), Constants::Messages::Chat::Commands::ANNOUNCE) && m_session.GetRole() == Role::Administrator)
        {
            auto message = Gx::StringHelper::Trim(text.substring(2));

            auto args = ChatSendEventArgs{ChatSendType::Announcement, message, sf::String()};
            if (Dispatch(ChatEvents::OnSend, args))
                return;

            m_service.SendAnnouncement(AnnouncementRequest{args.Message});
        }
        else if (GetParent<StateWaiting7K>() || GetParent<StatePlaying7K>())
        {
            auto args = ChatSendEventArgs{ChatSendType::Waiting, text, sf::String()};
            if (Dispatch(ChatEvents::OnSend, args))
                return;

            m_service.SendWaitingMessage(WaitingMessageRequest{args.Message});
        }
        else
        {
            auto args = ChatSendEventArgs{ChatSendType::MainRoom, text, sf::String()};
            if (Dispatch(ChatEvents::OnSend, args))
                return;

            m_service.SendMainRoomMessage(MainRoomMessageRequest{args.Message});
        }
    }

    ChatWindow* ChatPanel::GetChatWindow() const
    {
        return FindChild<ChatWindow>(Resource::ChatPanel::IDC_CHAT_WINDOW);
    }

    void ChatPanel::ShowChatButtonToolTip(const Gx::Control& sender, const sf::String& message)
    {
        if (const auto tooltip = FindChild<Gx::ToolTip>(Resource::ChatPanel::IDC_TOOLTIP_INFO))
        {
            tooltip->SetString(message);
            const auto position = sf::Vector2f{
                std::ceil(sender.GetParent()->GetPosition().x + (sender.GetPosition() / 2.f).x),
                std::ceil(sender.GetParent()->GetPosition().y - sender.GetPosition().y - tooltip->GetLocalBounds().size.y - tooltip->GetPadding().y),
            };

            if (sender.IsFocused())
                tooltip->Show(position, Gx::ToolTip::Alignment::Left);
            else
                tooltip->Hide();
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ChatPanel::SetInputEnabled(const bool enabled)
    {
        if (const auto chatBox = FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT); chatBox)
            chatBox->SetEnabled(enabled);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ChatPanel::SetMaximumTextLength(const unsigned int length)
    {
        if (const auto chatBox = FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT); chatBox)
            chatBox->SetMaximumTextLength(length);
    }
}
