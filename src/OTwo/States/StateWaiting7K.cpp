#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Data/Room.hpp>
#include <OTwo/Data/UserState.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>
#include <OTwo/States/Components/Waiting/MapSelector.hpp>

StateWaiting7K::StateWaiting7K(State &state) :
    State(state)
{
}

void StateWaiting7K::Initialize()
{
    State::Initialize();

    auto& app      = GetApplication();
    auto& director = GetDirector();
    auto& items    = app.Require<ItemFactory>();
    auto& mixer    = app.Require<Gx::Mixer>();
    auto& state    = app.Require<UserState>();
    auto& room     = state.GetRoomData();

    auto bgm         = Load<sf::Music>("STATE_WAITING/IDC_MUSIC");
    auto sfxNavigate = Load<sf::Sound>("STATE_WAITING/IDC_SOUND_07");
    auto sfxTeam     = Load<sf::Sound>("STATE_WAITING/IDC_SOUND_34");

    auto channelCategory = Load<Gx::Image>("STATE_WAITING/IDC_IMAGE_CHANNEL_CATEGORY");
    switch (state.GetMusicHall())
    {
        case MusicHall::Kalliope: channelCategory->SetFrame("Kalliope");  break;
        case MusicHall::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case MusicHall::Philix:   channelCategory->SetFrame("Philix");   break;
        case MusicHall::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case MusicHall::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case MusicHall::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }

    auto channelNumber = Load<Gx::Number>("STATE_WAITING/IDC_NUMBER_CHANNEL_ID");
    channelNumber->SetValue(state.GetChannelID());

    auto roomNumber = Load<Gx::Number>("STATE_WAITING/IDC_NUMBER_ROOM_ID");
    roomNumber->SetValue(room.ID);

    auto roomName = Load<Gx::Label>("STATE_WAITING/IDC_TEXT_ROOM_NAME");
    roomName->SetString(room.Title);

    auto musicName = Load<Gx::Label>("STATE_WAITING/IDC_TEXT_MUSIC_NAME");
    musicName->SetString(room.Chart.Title);

    std::string speedName(4, '\0');
    if (room.Speed > 0)
    {
        if (std::fmod(room.Speed, 1.0f) != 0)
            speedName.resize(std::snprintf(&speedName[0], speedName.size(), "%.1f", room.Speed));
        else
            speedName = std::to_string(static_cast<int>(room.Speed));
    }
    else
        speedName = "R";

    std::string diffName;
    switch (room.Difficulty)
    {
        case Difficulty::Easy:   diffName = "EX"; break;
        case Difficulty::Normal: diffName = "NX"; break;
        case Difficulty::Hard:   diffName = "HX"; break;
        case Difficulty::Master: diffName = "MX"; break;
    }

    auto level = Load<Gx::Image>("STATE_WAITING/IDC_IMAGE_ROOM_LEVEL");
    level->SetFrame(diffName + speedName);

    auto teamButtons = Load<Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_TEAM_BUTTONS");
    auto teamA = teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_A");
    teamA->SetCheckedState(true);

    for (auto node : teamButtons->GetChildren())
    {
        auto teamButton = dynamic_cast<Gx::RadioButton*>(node);
        if (!teamButton)
            continue;

        teamButton->SetCheckStateChangeCallback([=, &mixer] (auto sender)
        {
            if (!sender->IsChecked())
                return;

            mixer.Play(sfxTeam);
        });
    }

    auto mapSelector = Instantiate<MapSelector, Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_MAP_SELECTOR");
    mapSelector->Initialize();

    auto readyButton = Load<Gx::CheckBox>("STATE_WAITING/IDC_BUTTON_READY");
    readyButton->SetVisible(false);
    readyButton->SetEnabled(false);

    auto chatPanel  = Load<ChatPanel>("STATE_WAITING/IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);

    auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam!");
    chatWindow->PushSystemMessage("Let's play together~");

    //auto avatar = Load<Avatar>("STATE_WAITING/IDC_AVATAR");
    auto avatarList = Load<Gx::List>("STATE_WAITING/IDC_LIST_AVATAR");

    int index = 0;
    for (auto child : avatarList->GetChildren())
    {
        auto avatar = dynamic_cast<Avatar*>(child);
        if (!avatar)
            continue;

        auto player = state.GetPlayer();
        if (index == 2)
        {
            player = Player(player);
            player.Gender = Gender::Female;
        }
        avatar->SetPlayer(player);

        auto namePlate = avatar->FindChild<Gx::Image>("IDC_IMAGE_NAME_PLATE");
        if (namePlate)
            namePlate->SetColor(sf::Color::Green);

        if (index == 0 || index == 1 || index == 2)
        {
            for (auto [_, item]: items.GetDefaultItems(player.Gender))
                avatar->SetDefaultItem(item);

            if (auto item = items.GetItem(232); item)
                avatar->Equip(item);
        }
        else
        {
            if (auto item = items.GetItem(1534 + index); item)
                avatar->Equip(item);
        }

        index++;
    }

    auto btnBack = Load<Gx::Button>("STATE_WAITING/IDC_BUTTON_BACK");
    btnBack->SetClickCallback([&] (auto& sender, auto& ev) {
        director.Present<StateRoom>();
    });

    mixer.Play(bgm, "BGM");
}

