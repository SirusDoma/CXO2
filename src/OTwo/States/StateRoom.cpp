#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/Components/Common/Marquee.hpp>
#include <OTwo/States/Components/Common/ChatPanel.hpp>
#include <OTwo/States/Components/Room/RoomContainer.hpp>
#include <OTwo/States/Components/Room/UserList.hpp>
#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Models/Game.hpp>
#include <OTwo/Models/UserState.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Room.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>

StateRoom::StateRoom(State &state) :
    State(state)
{
}

void StateRoom::Initialize()
{
    State::Initialize();

    auto& director = GetDirector();
    auto& mixer    = Require<Gx::Mixer>();
    auto& state    = Require<UserState>();
    auto& items    = Require<ItemFactory>();

    auto bgm         = Load<sf::Music>("STATE_ROOM/IDC_MUSIC");
    auto sfxAccept   = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_02");
    auto sfxNavigate = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_07");
    auto sfxToggle   = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_14");

    auto player = state.GetCurrentPlayer();
    auto nicknameLabel = Load<Gx::Label>("STATE_ROOM/IDC_TEXT_NICKNAME");
    nicknameLabel->SetString("Lv." + std::to_string(player.Level) + ": " + player.Name);

    auto avatar = Load<Avatar>("STATE_ROOM/IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    auto notice = Load<Marquee>("IDC_TEXT_NOTICE");
    notice->SetString("Welcome to O2Jam! Let's play together~");

    auto channelCategory = Load<Gx::Image>("STATE_ROOM/IDC_IMAGE_CHANNEL_CATEGORY");
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

    auto channelNumber = Load<Gx::Number>("STATE_ROOM/IDC_NUMBER_CHANNEL_ID");
    channelNumber->SetValue(state.GetChannelID());

    auto chatPanel = Load<ChatPanel>("STATE_ROOM/IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);

    auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam");
    chatWindow->PushSystemMessage("/w Receiver   : Send message (whisper)");
    chatWindow->PushSystemMessage("F7            : Effect 2D/3D mode setting");
    chatWindow->PushSystemMessage("F8            : Cursor mode setting");
    chatWindow->PushSystemMessage("F9            : Toggle equalizer on/off");

    auto userList = Load<UserList>("STATE_ROOM/IDC_USER_LIST");
    userList->Initialize();

    auto users = std::vector<PlayerData>();
    userList->AddUser(state.GetCurrentPlayer());

    for (unsigned int i = 0; i < 34; i++)
        userList->AddUser(PlayerData{i + 3, "Dummy " + std::to_string(i), static_cast<int>(i) });

    auto roomContainer = Load<RoomContainer>("STATE_ROOM/IDC_ROOM_CONTAINER");
    roomContainer->Initialize();
    RoomData rooms[] = {
        RoomData{
            /* .ID           = */ 0,
            /* .RoomMasterID = */ 0,
            /* .Title        = */ "Let's play together~",
            /* .Chart        = */ ChartMetadataView
            {
                /* .ID           = */ "1",
                /* .New          = */ true,
                /* .Title        = */ "Earth Quake",
                /* .Artist       = */ "Kaze.o2SE",
                /* .NoteDesigner = */ "Kaze.o2SE",
                /* .BPM          = */ 150,
                /* .Genre        = */ "Rock",
                /* .Difficulty   = */ "HX",
                /* .Level        = */ 36,
            },
            /*.Difficulty    = */ Difficulty::HX,
            /*.GameMode      = */ GameMode::Versus,
            /*.SongMode      = */ SongMode::Normal,
            /*.State         = */ RoomState::Playing,
            /*.Speed         = */ 4.5f,
            /*.Locked        = */ false,
            /*.Capacity      = */ 8,
            /*.MinLevelLimit = */ 0,
            /*.MaxLevelLimit = */ 0,
            /*.Members       = */ { RoomMember{ 1 } }
        },
        RoomData{
            /* .ID            = */ 5,
            /* .RoomMasterID  = */ 0,
            /* .Title         = */ "Pimplex's room",
            /* .Chart         = */ ChartMetadataView{},
            /* .Difficulty    = */ Difficulty::HX,
            /* .GameMode      = */ GameMode::Single,
            /* .SongMode      = */ SongMode::Random,
            /* .State         = */ RoomState::Waiting,
            /* .Speed         = */ 3.5f,
            /* .Locked        = */ false,
            /* .Capacity      = */ 8,
            /* .MinLevelLimit = */ 0,
            /* .MaxLevelLimit = */ 0,
            /* .Members       = */ { RoomMember{ 2 } }
        },
        RoomData{
            /* .ID           = */ 3,
            /* .RoomMasterID = */ 0,
            /* .Title        = */ "kYo-Abhiem's room",
            /* .Chart        = */ ChartMetadataView
            {
                /*.ID           = */ "2",
                /*.New          = */ false,
                /*.Title        = */ "R3",
                /*.Artist       = */ "Kaze.o2SE",
                /*.NoteDesigner = */ "Kaze.o2SE",
                /*.BPM          = */ 150,
                /*.Genre        = */ "Rock",
                /*.Difficulty   = */ "HX",
                /*.Level        = */ 32,
            },
            /* .Difficulty    = */ Difficulty::HX,
            /* .GameMode      = */ GameMode::Versus,
            /* .SongMode      = */ SongMode::Normal,
            /* .State         = */ RoomState::Waiting,
            /* .Speed         = */ 4.f,
            /* .Locked        = */ false,
            /* .Capacity      = */ 8,
            /* .MinLevelLimit = */ 20,
            /* .MaxLevelLimit = */ 80,
            /* .Members       = */ { RoomMember{ 3 }, RoomMember{ 4 } }
        }
    };

    for (auto& room : rooms)
        roomContainer->PushRoomData(room);

    auto createRoomButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_CREATE_ROOM");
    if (auto dialog = Load<Gx::Dialog>("STATE_ROOM/IDC_DIALOG_CREATE_ROOM"); dialog)
    {
        auto createRoomDialog = Create<CreateRoomDialog>(*dialog);
        createRoomButton->SetClickCallback([=, &mixer, &state, &director] (auto& sender, auto& ev) {
            mixer.Play(sfxAccept, "SFX");
            createRoomDialog->Show(this, std::string(), false);
            createRoomDialog->SetAcceptCallback([&] () {
                auto musicList = state.GetInstalledMusic();
                state.SetCurrentRoom(RoomData{
                        4,
                        state.GetCurrentPlayer().ID,
                        createRoomDialog->GetRoomName(),
                        musicList[musicList.size() / 2].ToChartMetadataView(Difficulty::EX),
                        Difficulty::EX,
                        createRoomDialog->GetRoomMode(),
                        SongMode::Normal,
                        RoomState::Waiting,
                        1.0f,
                        !createRoomDialog->GetRoomPassword().empty(),
                        8,
                        createRoomDialog->GetMinLevelLimit(),
                        createRoomDialog->GetMaxLevelLimit(),
                        {
                            RoomMember{state.GetCurrentPlayer(), RoomTeam::A},
                            {},
                            {},
                            RoomMember{PlayerData{2, "DJZMO", 82, Gender::Male, 0, 0, false, {221}}, RoomTeam::F},
                            RoomMember{PlayerData{3, "kYo-Abhiem", 79, Gender::Male, 0, 0, false, {482}}, RoomTeam::G}
                        }
                });
                director.Present<StateWaiting7K>();
            });
        });
    }

    auto showAllButton     = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_SHOW_ALL");
    auto waitingRoomButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_SHOW_WAITING");

    showAllButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxToggle, "SFX");

        showAllButton->SetVisible(false);
        showAllButton->SetEnabled(false);

        waitingRoomButton->SetVisible(true);
        waitingRoomButton->SetEnabled(true);

        roomContainer->ShowWaitingOnly();
    });

    waitingRoomButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxToggle, "SFX");

        showAllButton->SetVisible(true);
        showAllButton->SetEnabled(true);

        waitingRoomButton->SetVisible(false);
        waitingRoomButton->SetEnabled(false);

        roomContainer->ShowAll();
    });

    auto roomLeftButton  = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_ROOM_LEFT");
    auto roomRightButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_ROOM_RIGHT");

    roomLeftButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxNavigate, "SFX");
        roomContainer->PreviousPage();
    });

    roomRightButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxNavigate, "SFX");
        roomContainer->NextPage();
    });

    if (auto dialog = Load<Gx::Dialog>("STATE_ROOM/IDC_DIALOG_OPTION"); dialog)
    {
        auto optionDialog = Create<OptionDialog>(*dialog);
        auto optionButton = FindChild<Gx::Button>("IDC_BUTTON_OPTION");
        optionButton->SetClickCallback([=] (auto& sender, auto& ev) {
            optionDialog->Show(this, std::string(), false);
        });
    }

    auto backButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_BACK");
    backButton->SetClickCallback([&](auto &, auto &) { OnBackClicked(); });

    mixer.Play(bgm, "BGM");
}

void StateRoom::OnBackClicked()
{
    auto& director = GetDirector();
    director.Present<StatePlanet>();
}
