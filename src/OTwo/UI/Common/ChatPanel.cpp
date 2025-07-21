#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Common/ChatWindow.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/State.hpp>

#include <OTwo/StringTable/Identifiers/ChatPanel.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/InputField.hpp>

using namespace StringTable::Identifiers;

ChatPanel::ChatPanel(SessionContext& session) :
    m_session(session)
{
}

void ChatPanel::Initialize()
{
    Gx::UiContainer::Initialize();

    const auto chatWindow = GetChatWindow();
    if (const auto controls = FindChild<Gx::UiContainer>(Resource::ChatPanel::IDC_CONTAINER_CHAT_SCROLL_CONTROLS))
    {
        const auto scrollChat = controls->FindChild<Gx::ScrollBar>(Resource::ChatPanel::IDC_SCROLL_BAR_CHAT);
        if (scrollChat)
            chatWindow->SetScrollBar(*scrollChat);

        if (const auto btnChatScrollUp = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_UP))
            btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });

        if (const auto btnChatScrollDown = controls->FindChild<Gx::Button>(Resource::ChatPanel::IDC_BUTTON_SCROLL_DOWN))
            btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });
    }

    const auto chatInput = FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT);
    chatInput->SetPermanentFocusEnabled(true);
    chatInput->SetTextEnteredCallback([=] (auto& sender, const sf::String& text)
    {
        chatWindow->PushMessage(m_session.GetCurrentPlayer(), text);
    });

    if (const auto chatButtonList = FindChild<Gx::List>(Resource::ChatPanel::IDC_LIST_CHAT_BUTTON))
    {
        const auto btnChatAll     = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_ALL);
        const auto btnChatFriend  = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_FRIEND);
        const auto btnChatGuild   = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_GUILD);
        const auto btnChatWhisper = chatButtonList->FindChild<Gx::RadioButton>(Resource::ChatPanel::IDC_RADIO_CHAT_WHISPER);

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
                if (const auto dialog = parent->Instantiate<Gx::Dialog>(Resource::ChatPanel::IDC_DIALOG_WHISPER); dialog)
                {
                    const auto nicknameInput = dialog->FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_NICKNAME);
                    nicknameInput->SetMaximumTextLength(20);
                    dialog->SetAcceptCallback([=, &radio]
                    {
                        if (nicknameInput->GetString().isEmpty())
                            fallbackCheckStateCallback(radio);
                    });

                    dialog->SetCancelCallback([=, &radio]
                    {
                        fallbackCheckStateCallback(radio);
                    });

                    auto ctx   = Gx::DialogPresentationContext();
                    ctx.Bounds = {{}, parent->GetView().getSize()};
                    ctx.Prompt = "Enter the nickname of person you wish to\nwhisper and then press the [OK] button.";

                    parent->Present(*dialog, ctx);
                    nicknameInput->SetString(std::string());
                    nicknameInput->SetFocus(true);
                    nicknameInput->SelectAll();
                }
            }
        });
    }
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
