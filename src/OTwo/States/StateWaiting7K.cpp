#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateLoading.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/Models/Game.hpp>

#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>
#include <OTwo/UI/Waiting/MapSelector.hpp>
#include <OTwo/UI/Waiting/InstrumentSelector.hpp>
#include <OTwo/UI/Dialogs/SelectMusicDialog.hpp>

#include <Genode/UI.hpp>
#include <magic_enum.hpp>
#include <Genode/Tasks/Sequence.hpp>

StateWaiting7K::StateWaiting7K(State &&state) :
    State(std::move(state))
{
}

void StateWaiting7K::Initialize()
{
    State::Initialize();

    auto& director  = GetDirector();
    auto& items     = Require<ItemFactory>();
    auto& mixer     = Require<Gx::Mixer>();
    auto& session   = Require<SessionContext>();
    auto& selection = Require<MusicSelectionContext>();
    auto& player    = session.GetCurrentPlayer();
    auto& room      = session.GetCurrentRoom();

    const auto bgm            = Instantiate<sf::Music>("STATE_WAITING/IDC_MUSIC");
    const auto sfxStart       = Instantiate<sf::Sound>("STATE_WAITING/IDC_SOUND_33");
    const auto sfxTeam        = Instantiate<sf::Sound>("STATE_WAITING/IDC_SOUND_34");
    const auto sfxSelectMusic = Instantiate<sf::Sound>("STATE_WAITING/IDC_SOUND_35");

    const auto channelCategory = Instantiate<Gx::Image>("STATE_WAITING/IDC_IMAGE_CHANNEL_CATEGORY");
    switch (session.GetMusicHall())
    {
        case MusicHall::Kalliope: channelCategory->SetFrame("Kalliope");  break;
        case MusicHall::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case MusicHall::Philix:   channelCategory->SetFrame("Philix");   break;
        case MusicHall::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case MusicHall::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case MusicHall::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }

    const auto channelNumber = Instantiate<Gx::Number>("STATE_WAITING/IDC_NUMBER_CHANNEL_ID");
    channelNumber->SetValue(session.GetChannelID());

    const auto roomNumber = Instantiate<Gx::Number>("STATE_WAITING/IDC_NUMBER_ROOM_ID");
    roomNumber->SetValue(room.ID);

    const auto roomName = Instantiate<Gx::Label>("STATE_WAITING/IDC_TEXT_ROOM_NAME");
    roomName->SetString(room.Title);

    const auto musicName = Instantiate<Gx::Label>("STATE_WAITING/IDC_TEXT_MUSIC_NAME");
    musicName->SetString(room.ChartMetadata.Title + " [BPM: " + Gx::StringHelper::ToString(room.ChartMetadata.BPM, 2) + "]");

    const auto level = Instantiate<Gx::Image>("STATE_WAITING/IDC_IMAGE_ROOM_LEVEL");
    level->SetFrame(room.GetRoomLevelCode());

    const auto avatarList = Instantiate<Gx::List>("STATE_WAITING/IDC_LIST_AVATAR");

    AvatarInfo *currentAvatarInfo = nullptr;
    auto currentMember = RoomMember();
    int memberIndex = 0;

    for (const auto child : avatarList->GetChildren())
    {
        if (memberIndex >= sizeof(room.Members) / sizeof(RoomMember))
            break;

        const auto avatar = dynamic_cast<Avatar*>(child);
        if (!avatar)
            continue;

        const auto avatarInfo = avatar->FindChild<AvatarInfo>("IDC_AVATAR_INFO");
        auto& member = room.Members[memberIndex];
        if (member.ID == 0)
        {
            avatar->ClearEquipments();
            avatarInfo->Reset();

            memberIndex++;
            continue;
        }

        if (member.ID == session.GetCurrentPlayer().ID)
        {
            currentMember = member;
            currentAvatarInfo = avatarInfo;
        }

        avatar->SetGender(member.Gender);
        avatarInfo->SetMember(const_cast<RoomMember&>(member));

        for (auto [_, item]: items.GetDefaultItems(member.Gender))
            avatar->SetDefaultItem(item);

        for (const auto itemID : member.EquippedItemIDs)
        {
            if (const auto item = items.GetItem(itemID); item)
                avatar->Equip(item);
        }

        memberIndex++;
    }

    const auto teamButtons = Instantiate<Gx::UiContainer>("STATE_WAITING/IDC_CONTAINER_TEAM_BUTTONS");
    auto teamButtonMatcher = [=] (RoomTeam team) -> Gx::RadioButton*
    {
        switch (team)
        {
            default:
            case RoomTeam::A: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_A");
            case RoomTeam::B: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_B");
            case RoomTeam::C: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_C");
            case RoomTeam::D: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_D");
            case RoomTeam::E: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_E");
            case RoomTeam::F: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_F");
            case RoomTeam::G: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_G");
            case RoomTeam::H: return teamButtons->FindChild<Gx::RadioButton>("IDC_RADIO_TEAM_H");
        }
    };

    if (const auto currentTeamButton = teamButtonMatcher(currentMember.Team); currentTeamButton)
        currentTeamButton->SetCheckedState(true);


    for (const auto team : {RoomTeam::A, RoomTeam::B, RoomTeam::C, RoomTeam::D, RoomTeam::E, RoomTeam::F, RoomTeam::G, RoomTeam::H })
    {
        const auto teamButton = teamButtonMatcher(team);
        if (!teamButton)
            continue;

        teamButton->SetCheckStateChangeCallback([=, &mixer] (auto sender)
        {
            if (!sender->IsChecked())
                return;

            if (currentAvatarInfo)
            {
                if (const auto member = currentAvatarInfo->GetMember(); member)
                    member->Team = team;

                currentAvatarInfo->Invalidate();
                mixer.Play(sfxTeam);
            }
        });
    }

    const auto mapSelector = Instantiate<MapSelector>("STATE_WAITING/IDC_CONTAINER_MAP_SELECTOR");
    mapSelector->Initialize();
    mapSelector->SetMapChangedCallback([=, s = &session, r = &room] (const unsigned int mapID)
    {
        auto data = Room(*r);
        data.MapID = mapID;

        s->SetCurrentRoom(data);
    });

    mapSelector->SetEffectChangedCallback([=, s = &session, r = &room] (const unsigned int effectID)
    {
        auto data = Room(*r);
        data.EffectID = effectID;

        s->SetCurrentRoom(data);
    });

    mapSelector->SetMapID(room.MapID, true);
    mapSelector->SetEffectID(room.EffectID);

    const auto instrumentSelector = Instantiate<InstrumentSelector>("STATE_WAITING/IDC_CONTAINER_INSTRUMENT_SELECTOR");
    instrumentSelector->Initialize();

    if (currentAvatarInfo)
    {
        instrumentSelector->SetInstrumentSelectCallack([=, &session, &room] (const Item *item)
        {

            if (const auto avatar = currentAvatarInfo->GetAvatar(); avatar)
            {
                auto data = Room(room);
                if (!item || avatar->IsEquiped(item))
                {
                    avatar->Unequip(EquipmentType::Bass);
                    avatar->Unequip(EquipmentType::Guitar);
                    avatar->Unequip(EquipmentType::Keyboard);
                    avatar->Unequip(EquipmentType::Drum);
                }
                else
                    avatar->Equip(item);

                for (auto& member : data.Members)
                {
                    if (member.ID != currentAvatarInfo->GetMember()->ID)
                        continue;

                    member.EquippedItemIDs.clear();
                    for (auto [_, equipedItem] : avatar->GetEquipedItems())
                        member.EquippedItemIDs.push_back(equipedItem->GetID());
                }

                session.SetCurrentRoom(data);
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

    const auto readyButton = Instantiate<Gx::CheckBox>("STATE_WAITING/IDC_BUTTON_READY");
    readyButton->SetVisible(false);
    readyButton->SetEnabled(false);

    const auto chatPanel  = Instantiate<ChatPanel>("STATE_WAITING/IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);

    const auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam!");
    chatWindow->PushSystemMessage("Let's play together~");

    if (const auto dialog = Instantiate<Gx::Dialog>("STATE_WAITING/IDC_DIALOG_SELECT_MUSIC"); dialog)
    {
        const auto selectMusicDialog = Create<SelectMusicDialog>(*dialog);
        selectMusicDialog->Initialize();
        if (const auto selectMusicButton = Instantiate<Gx::Button>("STATE_WAITING/IDC_BUTTON_SELECT_MUSIC"); selectMusicButton)
        {
            selectMusicButton->SetClickCallback([=, &mixer] (auto &sender, auto &ev)
            {
                mixer.Play(sfxSelectMusic);
                selectMusicDialog->Show(this, std::string(), false);
            });
        }

        selectMusicDialog->SetAcceptCallback([=, &session, &room] ()
        {
            auto data = Room(room);
            data.Difficulty = selectMusicDialog->GetSelectedDifficulty();
            data.Speed      = selectMusicDialog->GetSelectedSpeed();

            const auto music  = selectMusicDialog->GetSelectedMusic();
            const auto random = selectMusicDialog->GetSelectedRandomLevels() != static_cast<LevelCategory>(0);
            if (music.ID != 0)
            {
                const auto meta = music.ToChartMetadataView(selectMusicDialog->GetSelectedDifficulty());

                data.SongMode      = SongMode::Normal;
                data.ChartMetadata = meta;

                musicName->SetString(meta.Title + " [BPM: " + Gx::StringHelper::ToString(meta.BPM, 2) + "]");
            }
            else if (random)
            {
                data.SongMode = SongMode::Random;
                data.ChartMetadata = {};

                musicName->SetString("Random");
            }

            session.SetCurrentRoom(data);
            level->SetFrame(data.GetRoomLevelCode());
        });
    }


    const auto btnBack = Instantiate<Gx::Button>("STATE_WAITING/IDC_BUTTON_BACK");
    btnBack->SetClickCallback([&] (auto& sender, auto& ev) {
        director.Present<StateRoom>();
    });

    const auto btnStart = Instantiate<Gx::CheckBox>("STATE_WAITING/IDC_BUTTON_START");
    btnStart->SetCheckStateChangeCallback([=, &mixer, &director] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        sender->SetEnabled(false);
        btnBack->SetEnabled(false);
        mixer.Play(sfxStart);

        Run(Create<Gx::Sequence>([&director]
            {
                director.Present<StateLoading>();
            },
            Gx::Sequence::ListOf({ Create<Gx::Delay>(sf::milliseconds(100.f)) })
        ));
    });

    mixer.Play(bgm, "BGM");
}

void StateWaiting7K::OnKeyDown(const sf::Event::KeyEvent ev)
{
    State::OnKeyDown(ev);

    if (ev.code == sf::Keyboard::Key::F3)
    {
        if (const auto btnStart = Instantiate<Gx::CheckBox>("STATE_WAITING/IDC_BUTTON_START"))
            btnStart->SetCheckedState(true);
    }
}

