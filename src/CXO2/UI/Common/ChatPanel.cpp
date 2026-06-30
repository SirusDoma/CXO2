#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Common/ChatWindow.hpp>

#include <CXO2/States/State.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Services/MessagingService.hpp>

#include <CXO2/Utilities/StringFormatter.hpp>
#include <CXO2/StringTable/Identifiers/ChatPanel.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/InputField.hpp>
#include <CXO2/States/StateWaiting7K.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    ChatPanel::ChatPanel(SessionContext& session, MessagingService& service) :
        m_session(session),
        m_service(service)
    {
    }

    void ChatPanel::Initialize()
    {
        UiContainer::Initialize();

        const auto chatWindow = GetChatWindow();
        m_service.OnMessageReceive([=] (const auto& actor, const auto& text, const bool isWhisper)
        {
            auto parent = GetParent<Cx::State>();
            parent->Invoke([=]
            {
                if (!isWhisper)
                {
                    if (Gx::StringHelper::StartsWith(text, "/"))
                    {
                        if (const auto waiting = dynamic_cast<StateWaiting7K*>(parent))
                            waiting->OnMemberEmoticon(actor, text);
                    }
                    else
                        chatWindow->PushMessage(actor, text);
                }
                else
                    chatWindow->PushWhisper(actor, m_session.GetCharacterInfo(), text);
            });
        });

        if (const auto controls = FindChild<UiContainer>(Resource::ChatPanel::IDC_CONTAINER_CHAT_SCROLL_CONTROLS))
        {
            const auto scrollChat = controls->FindChild<Gx::ScrollBar>(Resource::ChatPanel::IDC_SCROLL_BAR_CHAT);
            if (scrollChat)
                chatWindow->SetScrollBar(*scrollChat);

            if (const auto btnChatScrollUp = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_UP))
                btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });

            if (const auto btnChatScrollDown = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_DOWN))
                btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });
        }

        if (const auto chatButtonList = FindChild<Gx::List>(Resource::ChatPanel::IDC_LIST_CHAT_BUTTON))
        {
            const auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_ALL);
            const auto btnChatFriend  = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_FRIEND);
            const auto btnChatGuild   = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_GUILD);
            const auto btnChatWhisper = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_WHISPER);

            btnChatAll->SetCheckedState(true);
            btnChatAll->SetCheckStateChangeCallback([=] (const Gx::RadioButton& radio)
            {
                if (radio.IsChecked())
                    m_recipient = {};
            });

            const auto fallbackCheckStateCallback = [=] (const Gx::RadioButton& radio)
            {
                if (radio.IsChecked())
                    btnChatAll->SetCheckedState(true);
            };

            btnChatFriend->SetCheckStateChangeCallback(fallbackCheckStateCallback);
            btnChatGuild->SetCheckStateChangeCallback(fallbackCheckStateCallback);
            btnChatWhisper->SetCheckStateChangeCallback([=] (auto& radio)
            {
                const auto parent = GetParent<Cx::State>();
                if (!parent)
                    return;

                if (radio.IsChecked())
                {
                    if (const auto dialog = parent->Instantiate<Gx::Dialog>(Resource::ChatPanel::IDC_DIALOG_WHISPER); dialog)
                    {
                        const auto nicknameInput = dialog->FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_NICKNAME);
                        if (nicknameInput)
                        {
                            nicknameInput->SetMaximumTextLength(20);
                            nicknameInput->SetString(m_recipient);
                            nicknameInput->SetFocus(true);
                            nicknameInput->SelectAll();
                        }

                        dialog->SetAcceptCallback([=, &radio]
                        {
                            if (nicknameInput)
                            {
                                if (nicknameInput->GetString().isEmpty())
                                    fallbackCheckStateCallback(radio);

                                m_recipient = nicknameInput->GetString();
                            }
                        });

                        dialog->SetCancelCallback([=, &radio]
                        {
                            fallbackCheckStateCallback(radio);
                        });

                        auto ctx   = Gx::DialogPresentationContext();
                        ctx.Bounds = {{}, parent->GetView().getSize()};
                        ctx.Prompt = "Enter the nickname of person you wish to\nwhisper and then press the [OK] button.";

                        parent->Present(*dialog, ctx);
                    }
                }
            });
        }

        const auto chatInput = FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT);
        chatInput->SetPermanentFocusEnabled(true);
        chatInput->SetTextEnteredCallback([=] (auto& sender, const sf::String& text)
        {
            if (Gx::StringHelper::StartsWith(text, "/w"))
            {
                const auto tokens = Gx::StringHelper::Split(text);
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


                auto message = Gx::StringHelper::Trim(text.substring(3 + tokens[1].size()));
                m_service.SendWhisper(tokens[1], message, [=] (const bool success)
                {
                    if (success)
                        chatWindow->PushWhisper(m_session.GetCharacterInfo(), CharacterInfo{m_recipient}, message);
                    else
                        chatWindow->PushSystemMessage(fmt::format("The message was not delivered to {}", tokens[1]));
                });
            }
            else if (!m_recipient.isEmpty())
            {
                m_service.SendWhisper(m_recipient, text, [=] (const bool success)
                {
                    if (success)
                        chatWindow->PushWhisper(m_session.GetCharacterInfo(), CharacterInfo{m_recipient}, text);
                    else
                        chatWindow->PushSystemMessage(sf::String(fmt::format(L"The message was not delivered to {}", m_recipient)));
                });
            }
            else if (Gx::StringHelper::StartsWith(text, "/n") && m_session.GetCharacterInfo().Role == Role::Administrator)
                m_service.SendAnnouncement(Gx::StringHelper::Trim(text.substring(2)));
            else
                m_service.SendMessage(text);
        });
    }

    void ChatPanel::Finalize()
    {
        UiContainer::Finalize();
        m_service.UnsubscribeEvents();
    }

    ChatWindow* ChatPanel::GetChatWindow() const
    {
        return FindChild<ChatWindow>(Resource::ChatPanel::IDC_CHAT_WINDOW);
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
