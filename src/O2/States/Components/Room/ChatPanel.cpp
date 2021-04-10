#include <O2/States/Components/Room/ChatPanel.hpp>

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
    auto chatWindow = scene.Create<ChatWindow>("Metadata/State/Room/ChatWindow.json");
    auto scrollChat = scene.Create<Gx::ScrollBar>("Metadata/State/Room/ChatScroll.json");
    chatWindow->SetScrollBar(*scrollChat);

    auto systemPlayer = PlayerInfo{0, -1, sf::String(), true};
    chatWindow->PushMessage(systemPlayer, "Welcome to O2Jam");
    chatWindow->PushMessage(systemPlayer, "/w Receiver   : Send message (whisper).");
    chatWindow->PushMessage(systemPlayer, "F8                 : Toggle windows/image cursor mode.");
    chatWindow->PushMessage(systemPlayer, "F9                 : Toggle equalizer on/off.");

    auto btnChatScrollUp = scene.Create<Gx::Button>("Metadata/State/Room/Btn_ChatScrollUp.json");
    btnChatScrollUp->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Decrease(); });
    auto btnChatScrollDown = scene.Create<Gx::Button>("Metadata/State/Room/Btn_ChatScrollDown.json");
    btnChatScrollDown->SetClickCallback([=] (auto& sender, auto& ev) { scrollChat->Increase(); });
    AddChild(chatWindow, scrollChat, btnChatScrollUp, btnChatScrollDown);

    auto chatBox = scene.Create<Gx::TextBox>("Metadata/State/Room/ChatBox.json");
    chatBox->SetTextEnteredCallback([=] (auto& textBox, sf::String text)
    {
        std::cout << std::string(text) << std::endl;
        chatWindow->PushMessage(PlayerInfo{1, -1, "CXO2", false}, text);
    });
    AddChild(chatBox);

    auto btnChatAll     = scene.Create<Gx::RadioButton>("Metadata/State/Room/Btn_ChatAll.json");
    auto btnChatFriend  = scene.Create<Gx::RadioButton>("Metadata/State/Room/Btn_ChatFriend.json");
    auto btnChatGuild   = scene.Create<Gx::RadioButton>("Metadata/State/Room/Btn_ChatGuild.json");
    auto btnChatWhisper = scene.Create<Gx::RadioButton>("Metadata/State/Room/Btn_ChatWhisper.json");

    auto chatButtonList = scene.Create<Gx::List>("Metadata/State/Room/ChatButtonList.json");
    chatButtonList->AddChild(btnChatAll, btnChatFriend, btnChatGuild, btnChatWhisper);
    btnChatAll->SetCheckedState(true);

    AddChild(chatButtonList);
}
