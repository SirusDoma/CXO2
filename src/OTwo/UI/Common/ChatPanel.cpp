#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Common/ChatWindow.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

ChatPanel::ChatPanel(SessionContext& session) :
    m_session(session)
{
}

void ChatPanel::Initialize()
{
    Gx::UiContainer::Initialize();

    const auto chatWindow = GetChatWindow();
    const auto scrollChat = FindChild<Gx::ScrollBar>("IDC_SCROLL_BAR_CHAT");
    // scrollChat->SetValueChangedCallback([=] (auto& sender, float value) { chatWindow->SetScrollOffset(static_cast<unsigned int>(value)); });
    if (scrollChat)
        chatWindow->SetScrollBar(*scrollChat);

    if (const auto btnChatScrollUp = FindChild<Gx::Button>("IDC_BUTTON_SCROLL_UP"))
        btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });

    if (const auto btnChatScrollDown = FindChild<Gx::Button>("IDC_BUTTON_SCROLL_DOWN"))
        btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });

    const auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT");
    chatBox->SetPermanentFocusEnabled(true);
    chatBox->SetTextEnteredCallback([=] (auto& textBox, const sf::String& text)
    {
        chatWindow->PushMessage(m_session.GetCurrentPlayer(), text);
    });

    if (const auto chatButtonList = FindChild<Gx::List>("IDC_LIST_CHAT_BUTTON"))
    {
        const auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>("IDC_RADIO_CHAT_ALL");
        const auto btnChatFriend  = chatButtonList->FindChild<Gx::RadioButton>("IDC_RADIO_CHAT_FRIEND");
        const auto btnChatGuild   = chatButtonList->FindChild<Gx::RadioButton>("IDC_RADIO_CHAT_GUILD");
        const auto btnChatWhisper = chatButtonList->FindChild<Gx::RadioButton>("IDC_RADIO_CHAT_WHISPER");

        btnChatAll->SetCheckedState(true);
        const auto fallbackCheckStateCallback = [=] (const Gx::RadioButton& radio)
        {
            if (radio.IsChecked())
                btnChatAll->SetCheckedState(true);
        };

        btnChatFriend->SetCheckStateChangeCallback(fallbackCheckStateCallback);
        btnChatGuild->SetCheckStateChangeCallback(fallbackCheckStateCallback);
        btnChatWhisper->SetCheckStateChangeCallback([=] (auto& radio)
        {
            const auto parent = GetParent<::State>();
            if (!parent)
                return;

            if (radio.IsChecked())
            {
                if (const auto dialog = parent->Instantiate<Gx::Dialog>("IDC_DIALOG_WHISPER"); dialog)
                {
                    const auto nicknameTextBox = dialog->FindChild<Gx::TextBox>("IDC_EDIT_NICKNAME");
                    nicknameTextBox->SetMaximumTextLength(20);
                    dialog->SetAcceptCallback([=]
                    {
                        if (nicknameTextBox->GetString().isEmpty())
                            fallbackCheckStateCallback(radio);
                    });

                    dialog->SetCancelCallback([=]
                    {
                        fallbackCheckStateCallback(radio);
                    });

                    dialog->Show(parent, "Enter the nickname of person you wish to\nwhisper and then press the [OK] button.", false);
                    nicknameTextBox->SetString(std::string());
                    nicknameTextBox->SetFocus(true);
                    nicknameTextBox->SelectAll();
                }
            }
        });
    }
}

ChatWindow* ChatPanel::GetChatWindow() const
{
    return FindChild<ChatWindow>("IDC_CHAT_WINDOW");
}

void ChatPanel::SetInputEnabled(const bool enabled)
{
    if (const auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT"); chatBox)
        chatBox->SetEnabled(enabled);
}

void ChatPanel::SetMaximumTextLength(const unsigned int length)
{
    if (const auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT"); chatBox)
        chatBox->SetMaximumTextLength(length);
}
