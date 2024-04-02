#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Data/Room.hpp>
#include <OTwo/Data/UserState.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/States/Components/Waiting/AvatarInfo.hpp>
#include <OTwo/States/Components/Waiting/MapSelector.hpp>
#include <OTwo/States/Components/Waiting/InstrumentSelector.hpp>

#include <Genode/UI.hpp>

#include <magic_enum.hpp>
#include <OTwo/States/Components/Dialogs/SelectMusicDialog.hpp>

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
    auto& player   = state.GetCurrentPlayer();
    auto& room     = state.GetRoomData();

    auto bgm            = Load<sf::Music>("STATE_WAITING/IDC_MUSIC");
    auto sfxNavigate    = Load<sf::Sound>("STATE_WAITING/IDC_SOUND_07");
    auto sfxTeam        = Load<sf::Sound>("STATE_WAITING/IDC_SOUND_34");
    auto sfxSelectMusic = Load<sf::Sound>("STATE_WAITING/IDC_SOUND_35");

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
    musicName->SetString(room.Chart.Title + " [BPM: " + Gx::StringHelper::ToString(room.Chart.BPM, 2) + "]");

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

    auto avatarList = Load<Gx::List>("STATE_WAITING/IDC_LIST_AVATAR");

    AvatarInfo *currentAvatarInfo = nullptr;
    auto currentMember = RoomMember();
    int memberIndex = 0;

    for (auto child : avatarList->GetChildren())
    {
        if (memberIndex >= sizeof(room.Members) / sizeof(RoomMember))
            break;

        auto avatar = dynamic_cast<Avatar*>(child);
        if (!avatar)
            continue;

        auto avatarInfo = avatar->FindChild<AvatarInfo>("IDC_AVATAR_INFO");
        auto& member = room.Members[memberIndex];
        if (member.ID == 0)
        {
            avatar->ClearEquipments();
            avatarInfo->Reset();

            memberIndex++;
            continue;
        }

        if (member.ID == state.GetCurrentPlayer().ID)
        {
            currentMember = member;
            currentAvatarInfo = avatarInfo;
        }

        avatar->SetGender(member.Gender);
        avatarInfo->SetMember(const_cast<RoomMember&>(member));

        for (auto [_, item]: items.GetDefaultItems(member.Gender))
            avatar->SetDefaultItem(item);

        for (auto itemID : member.EquippedItemIDs)
        {
            if (auto item = items.GetItem(itemID); item)
                avatar->Equip(item);
        }

        memberIndex++;
    }

    auto teamButtons = Load<Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_TEAM_BUTTONS");
    auto teamButtonMatcher = [=] (RoomTeam team) -> Gx::RadioButton*
    {
        switch (team)
        {
            default:
            case A: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_A");
            case B: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_B");
            case C: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_C");
            case D: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_D");
            case E: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_E");
            case F: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_F");
            case G: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_G");
            case H: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_H");
        }
    };

    if (auto currentTeamButton = teamButtonMatcher(currentMember.Team); currentTeamButton)
        currentTeamButton->SetCheckedState(true);


    for (auto team : {RoomTeam::A, RoomTeam::B, RoomTeam::C, RoomTeam::D, RoomTeam::E, RoomTeam::F, RoomTeam::G, RoomTeam::H })
    {
        auto teamButton = teamButtonMatcher(team);
        if (!teamButton)
            continue;

        teamButton->SetCheckStateChangeCallback([=, &mixer] (auto sender)
        {
            if (!sender->IsChecked())
                return;

            if (currentAvatarInfo)
            {
                if (auto member = currentAvatarInfo->GetMember(); member)
                    member->Team = team;

                currentAvatarInfo->Invalidate();
                mixer.Play(sfxTeam);
            }
        });
    }

    auto mapSelector = Instantiate<MapSelector, Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_MAP_SELECTOR");
    mapSelector->Initialize();

    auto instrumentSelector = Instantiate<InstrumentSelector, Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_INSTRUMENT_SELECTOR");
    instrumentSelector->Initialize();

    if (currentAvatarInfo)
    {
        instrumentSelector->SetInstrumentSelectCallack([=] (Item *item)
        {
            if (auto avatar = currentAvatarInfo->GetAvatar(); avatar)
            {
                if (!item || avatar->IsEquiped(item))
                {
                    avatar->Unequip(EquipmentType::Bass);
                    avatar->Unequip(EquipmentType::Guitar);
                    avatar->Unequip(EquipmentType::Keyboard);
                    avatar->Unequip(EquipmentType::Drum);
                }
                else
                    avatar->Equip(item);
            }
        });
    }

    instrumentSelector->AddInstrument(items.GetItem(232));
    instrumentSelector->AddInstrument(items.GetItem(233));
    instrumentSelector->AddInstrument(items.GetItem(234));
    instrumentSelector->AddInstrument(items.GetItem(39));
    instrumentSelector->AddInstrument(items.GetItem(238));
    instrumentSelector->AddInstrument(items.GetItem(255));
    instrumentSelector->AddInstrument(items.GetItem(256));
    instrumentSelector->AddInstrument(items.GetItem(257));
    instrumentSelector->AddInstrument(items.GetItem(304));
    instrumentSelector->AddInstrument(items.GetItem(410));
    instrumentSelector->AddInstrument(items.GetItem(411));
    instrumentSelector->AddInstrument(items.GetItem(412));
    instrumentSelector->AddInstrument(items.GetItem(1429));

    auto readyButton = Load<Gx::CheckBox>("STATE_WAITING/IDC_BUTTON_READY");
    readyButton->SetVisible(false);
    readyButton->SetEnabled(false);

    auto chatPanel  = Load<ChatPanel>("STATE_WAITING/IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);

    auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam!");
    chatWindow->PushSystemMessage("Let's play together~");

    if (auto dialogSelectMusic = Instantiate<SelectMusicDialog, Gx::Dialog>("STATE_WAITING/IDC_DIALOG_SELECT_MUSIC"); dialogSelectMusic)
    {
        dialogSelectMusic->Initialize();
        if (auto selectMusicButton = Load<Gx::Button>("STATE_WAITING/IDC_BUTTON_SELECT_MUSIC"); selectMusicButton)
        {
            selectMusicButton->SetClickCallback([=, &mixer] (auto &sender, auto &ev)
            {
                mixer.Play(sfxSelectMusic);
                dialogSelectMusic->Show(this, std::string(), false);
            });
        }
    }


    auto btnBack = Load<Gx::Button>("STATE_WAITING/IDC_BUTTON_BACK");
    btnBack->SetClickCallback([&] (auto& sender, auto& ev) {
        director.Present<StateRoom>();
    });

    mixer.Play(bgm, "BGM");
}

