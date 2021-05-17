#include <O2/States/Components/Common/ChatPanel.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

#include <O2/States/Components/Common/ChatWindow.hpp>
#include <iostream>

ChatPanel::ChatPanel()
{
}

void ChatPanel::Initialize(Gx::Scene &scene)
{
    // TODO determine which assets to use for State Room and State Waiting
    auto chatWindow = scene.Create<ChatWindow>("Interface/Metadata/State/Room/ChatPanel/ChatWindow.json");
    auto systemPlayer = Room::PlayerInfo{0, -1, sf::String(), true};

    auto scrollChat = scene.Create<Gx::ScrollBar>("Interface/Metadata/State/Room/ChatPanel/ChatScroll.json");
    scrollChat->SetValueChangedCallback([=] (auto& sender, float value) { chatWindow->SetScrollOffset(static_cast<unsigned int>(value)); });

    chatWindow->PushMessage(systemPlayer, "Welcome to O2Jam");
    chatWindow->PushMessage(systemPlayer, "/w Receiver   : Send message (whisper)");
    chatWindow->PushMessage(systemPlayer, "F7            : Effect 2D/3D mode setting");
    chatWindow->PushMessage(systemPlayer, "F8            : Cursor mode setting");
    chatWindow->PushMessage(systemPlayer, "F9            : Toggle equalizer on/off");

    auto btnChatScrollUp = scene.Create<Gx::Button>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatScrollUp.json");
    auto btnChatScrollDown = scene.Create<Gx::Button>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatScrollDown.json");
    btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });
    btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });
    AddChild(chatWindow, scrollChat, btnChatScrollUp, btnChatScrollDown);

    auto chatBox = scene.Create<Gx::TextBox>("Interface/Metadata/State/Room/ChatPanel/ChatBox.json");
    chatBox->SetPermanentFocusEnabled(true);
    chatBox->SetTextEnteredCallback([=] (auto& textBox, sf::String text)
    {
        std::cout << std::string(text) << std::endl;
        chatWindow->PushMessage(Room::PlayerInfo{1, -1, "CXO2", false}, text);
    });
    AddChild(chatBox);

    auto btnChatAll     = scene.Create<Gx::RadioButton>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatAll.json");
    auto btnChatFriend  = scene.Create<Gx::RadioButton>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatFriend.json");
    auto btnChatGuild   = scene.Create<Gx::RadioButton>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatGuild.json");
    auto btnChatWhisper = scene.Create<Gx::RadioButton>("Interface/Metadata/State/Room/ChatPanel/Btn_ChatWhisper.json");

    auto chatButtonList = scene.Create<Gx::List>("Interface/Metadata/State/Room/ChatPanel/ChatButtonList.json");
    chatButtonList->AddChild(btnChatAll, btnChatFriend, btnChatGuild, btnChatWhisper);
    btnChatAll->SetCheckedState(true);

    AddChild(chatButtonList);
}
