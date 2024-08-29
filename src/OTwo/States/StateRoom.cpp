#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/UI/Common/Marquee.hpp>
#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Room/RoomContainer.hpp>
#include <OTwo/UI/Room/UserList.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/MusicSelectionContext.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Room.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <OTwo/States/StateMyRoom.hpp>

StateRoom::StateRoom(State &&state) :
    State(std::move(state)),
    m_optionDialog(),
    m_createRoomDialog()
{
}

void StateRoom::Initialize()
{
    State::Initialize();

    auto& director       = GetDirector();
    auto& mixer          = Require<Gx::Mixer>();
    auto& session        = Require<SessionContext>();
    auto& selection      = Require<MusicSelectionContext>();
    const auto& items    = Require<ItemFactory>();

    const auto bgm         = Instantiate<sf::Music>("BGM/bgRoom.ogg");
    const auto sfxAccept   = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxNavigate = Instantiate<sf::Sound>("bgEffect/07");
    const auto sfxToggle   = Instantiate<sf::Sound>("bgEffect/14");

    const auto player = session.GetCurrentPlayer();
    const auto nicknameLabel = Instantiate<Gx::Label>("IDC_TEXT_NICKNAME");
    nicknameLabel->SetString("Lv." + std::to_string(player.Level) + ": " + player.Name);

    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    for (const auto id : player.Inventory)
    {
        if (const auto item = items.GetItem(id); item)
            avatar->Equip(item);
    }

    const auto notice = Instantiate<Marquee>("IDC_TEXT_NOTICE");
    notice->SetString("Welcome to O2Jam! Let's play together~");

    const auto channelCategory = Instantiate<Gx::Image>("IDC_IMAGE_CHANNEL_CATEGORY");
    switch (session.GetMusicHall())
    {
        case MusicHall::Kalliope: channelCategory->SetFrame("Kalliope"); break;
        case MusicHall::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case MusicHall::Philix:   channelCategory->SetFrame("Philix");   break;
        case MusicHall::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case MusicHall::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case MusicHall::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }

    const auto channelNumber = Instantiate<Gx::Number>("IDC_NUMBER_CHANNEL_ID");
    channelNumber->SetValue(session.GetChannelID());

    const auto chatPanel = Instantiate<ChatPanel>("IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);

    const auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam");
    chatWindow->PushSystemMessage("Alt+Enter     : Toggle windowed/fullscreen mode setting");
    chatWindow->PushSystemMessage("/w Receiver  : Send message (whisper)");
    chatWindow->PushSystemMessage("F7                 : Effect 2D/3D mode setting");
    chatWindow->PushSystemMessage("F8                 : Cursor mode setting");
    chatWindow->PushSystemMessage("F9                 : Toggle equalizer on/off");

    const auto userList = Instantiate<UserList>("IDC_USER_LIST");
    userList->Initialize();

    auto users = std::vector<Player>();
    userList->AddUser(session.GetCurrentPlayer());

    for (unsigned int i = 0; i < 34; i++)
        userList->AddUser(Player{i + 3, "Dummy " + std::to_string(i), static_cast<int>(i) });

    const auto roomContainer = Instantiate<RoomContainer>("IDC_ROOM_CONTAINER");
    roomContainer->Initialize();
    Room rooms[] = {
        Room{
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
        Room{
            /* .ID            = */ 5,
            /* .RoomMasterID  = */ 0,
            /* .Title         = */ "Another's room",
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
        Room{
            /* .ID           = */ 3,
            /* .RoomMasterID = */ 0,
            /* .Title        = */ "Someone's room",
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
        roomContainer->Add(room);

    const auto createRoomButton = Instantiate<Gx::Button>("IDC_BUTTON_CREATE_ROOM");
    if (const auto dialog = Instantiate<Gx::Dialog>("IDC_DIALOG_CREATE_ROOM"); dialog)
    {
        auto createRoomDialog = Create<CreateRoomDialog>(*dialog);
        createRoomButton->SetClickCallback([=, &mixer, &session, &director] (auto& sender, auto& ev) {
            mixer.Play(sfxAccept, "SFX");
            createRoomDialog->Show(this, std::string(), false);
            createRoomDialog->SetAcceptCallback([&] () {
                const auto musicList = session.GetInstalledMusic();
                const auto music = !selection.GetMetadata().Source.empty() ? selection.GetMetadata() : musicList[musicList.size() - 1];
                session.SetCurrentRoom(Room{
                    4,
                    session.GetCurrentPlayer().ID,
                    createRoomDialog->GetRoomName(),
                    music.ToChartMetadataView(selection.GetDifficulty()),
                    selection.GetDifficulty(),
                    createRoomDialog->GetRoomMode(),
                    SongMode::Normal,
                    RoomState::Waiting,
                    selection.GetSpeed(),
                    !createRoomDialog->GetRoomPassword().empty(),
                    8,
                    createRoomDialog->GetMinLevelLimit(),
                    createRoomDialog->GetMaxLevelLimit(),
                    {
                        RoomMember{session.GetCurrentPlayer(), RoomTeam::A, sf::Color::Transparent, 0, true},
                        {},
                        {},
                        RoomMember{Player{2, "Random #1", 82, Gender::Male, 0, 0, false, {221, 304}}, RoomTeam::F, sf::Color::Transparent, 0, true},
                        {},
                        {},
                        {},
                        RoomMember{Player{3, "Random #2", 79, Gender::Male, 0, 0, false, {39}}, RoomTeam::G, sf::Color::Transparent, 0, false}
                    }
                });
                director.Present<StateWaiting7K>();
            });
        });
    }

    const auto showAllButton     = Instantiate<Gx::Button>("IDC_BUTTON_SHOW_ALL");
    const auto waitingRoomButton = Instantiate<Gx::Button>("IDC_BUTTON_SHOW_WAITING");

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

    const auto roomLeftButton  = Instantiate<Gx::Button>("IDC_BUTTON_ROOM_LEFT");
    const auto roomRightButton = Instantiate<Gx::Button>("IDC_BUTTON_ROOM_RIGHT");

    roomLeftButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxNavigate, "SFX");
        roomContainer->PreviousPage();
    });

    roomRightButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxNavigate, "SFX");
        roomContainer->NextPage();
    });

    const auto myRoomButton = Instantiate<Gx::Button>("IDC_BUTTON_MY_ROOM");
    myRoomButton->SetClickCallback([this](auto &, auto &) { OnMyRoomClicked(); });

    if (const auto dialog = Instantiate<Gx::Dialog>("IDC_DIALOG_OPTION"); dialog)
    {
        const auto optionDialog = Create<OptionDialog>(*dialog);
        const auto optionButton = FindChild<Gx::Button>("IDC_BUTTON_OPTION");
        optionButton->SetClickCallback([=] (auto& sender, auto& ev) {
            optionDialog->Show(this, std::string(), false);
        });
    }

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([&](auto &, auto &) { OnBackClicked(); });

    bgm->setLoop(true);
    mixer.Play(bgm, "BGM");
}

void StateRoom::OnMyRoomClicked() const
{
    auto& director = GetDirector();
    director.Present<StateMyRoom>();
}

void StateRoom::OnBackClicked() const
{
    auto& director = GetDirector();
    director.Present<StatePlanet>();
}
