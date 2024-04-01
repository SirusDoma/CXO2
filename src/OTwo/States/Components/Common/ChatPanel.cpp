#include <OTwo/States/Components/Common/ChatPanel.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>
#include <OTwo/Data/UserState.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

ChatPanel::ChatPanel()
{
}

void ChatPanel::Initialize()
{
    auto chatWindow = GetChatWindow();
    auto scrollChat = FindChild<Gx::ScrollBar>("IDC_SCROLL_BAR_CHAT");
    // scrollChat->SetValueChangedCallback([=] (auto& sender, float value) { chatWindow->SetScrollOffset(static_cast<unsigned int>(value)); });
    if (scrollChat)
        chatWindow->SetScrollBar(*scrollChat);

    auto btnChatScrollUp = FindChild<Gx::Button>("IDC_BUTTON_SCROLL_UP");
    auto btnChatScrollDown = FindChild<Gx::Button>("IDC_BUTTON_SCROLL_DOWN");
    btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });
    btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });

    auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT");
    chatBox->SetPermanentFocusEnabled(true);
    chatBox->SetTextEnteredCallback([=] (auto& textBox, const sf::String &text)
    {
        chatWindow->PushMessage(Gx::Application::Instance().Require<UserState>().GetCurrentPlayer(), text);
    });

    auto chatButtonList = FindChild<Gx::List>("IDC_LIST_CHAT_BUTTON");
    auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>("IDC_RADIO_CHAT_ALL");
    btnChatAll->SetCheckedState(true);
}

ChatWindow *ChatPanel::GetChatWindow() const
{
    return FindChild<ChatWindow>("IDC_CHAT_WINDOW");
}

void ChatPanel::SetInputEnabled(bool enabled)
{
    if (auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT"); chatBox)
        chatBox->SetEnabled(enabled);
}

void ChatPanel::SetMaximumTextLength(unsigned int length)
{
    if (auto chatBox = FindChild<Gx::TextBox>("IDC_EDIT_CHAT"); chatBox)
        chatBox->SetMaximumTextLength(length);
}
