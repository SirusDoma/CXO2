#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/Data/UserState.hpp>
#include <OTwo/States/Components/Common/Marquee.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

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
    auto sfxAccept   = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_ACCEPT");
    auto sfxNavigate = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_NAVIGATION");
    auto sfxToggle   = Load<sf::Sound>("STATE_ROOM/IDC_SOUND_TOGGLE");

    auto player = state.GetPlayer();
    auto nicknameLabel = Load<Gx::Label>("STATE_ROOM/IDC_TEXT_NICKNAME");
    nicknameLabel->SetString("Lv." + std::to_string(player.Level) + ": " + player.Name);

    auto avatar = Load<Avatar>("STATE_ROOM/IDC_AVATAR");
    avatar->SetPlayer(player);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    auto notice = Load<Marquee>("IDC_MARQUEE_NOTICE");
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

    auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam");
    chatWindow->PushSystemMessage("/w Receiver   : Send message (whisper)");
    chatWindow->PushSystemMessage("F7            : Effect 2D/3D mode setting");
    chatWindow->PushSystemMessage("F8            : Cursor mode setting");
    chatWindow->PushSystemMessage("F9            : Toggle equalizer on/off");

    auto userList = Load<UserList>("STATE_ROOM/IDC_USER_LIST");
    userList->Initialize();

    auto users = std::vector<Player>();
    userList->AddUser(state.GetPlayer());

    for (unsigned int i = 0; i < 34; i++)
        userList->AddUser(Player{i + 3, 1, "Dummy " + std::to_string(i)});

    auto roomContainer = Load<RoomContainer>("STATE_ROOM/IDC_ROOM_CONTAINER");
    roomContainer->Initialize();
    RoomData rooms[] = {
        RoomData{
            0,
            "Let's play together~",
            ChartMetadata{"Earth Quake", "Kaze.o2SE", "Kaze.o2SE", "Rock", 36},
            Difficulty::Hard,
            GameMode::Versus,
            SongMode::Normal,
            RoomState::Playing,
            4.5f,
            false,
            2
        },
        RoomData{
            5,
            "Pimplex's room",
            ChartMetadata{},
            Difficulty::Hard,
            GameMode::Single,
            SongMode::Random,
            RoomState::Waiting,
            3.5f,
            false,
        },
        RoomData{
            3,
            "kYo-Abhiem's room",
            ChartMetadata{"R3", "Kaze.o2SE", "Kaze.o2SE", "Rock", 32},
            Difficulty::Hard,
            GameMode::Versus,
            SongMode::Normal,
            RoomState::Waiting,
            4.f,
            false,
        }
    };

    for (auto room : rooms)
        roomContainer->PushRoomData(room);

    auto createRoomButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_CREATE_ROOM");
    if (auto dialog = Load<Gx::Dialog>("STATE_ROOM/IDC_DIALOG_CREATE_ROOM"); dialog)
    {
        auto createRoomDialog = Create<CreateRoomDialog>(*dialog);
        createRoomButton->SetClickCallback([=, &mixer, &state, &director] (auto& sender, auto& ev) {
            mixer.Play(sfxAccept, "SFX");
            createRoomDialog->Show(this, std::string(), false);
            createRoomDialog->SetAcceptCallback([&] () {
                state.SetRoomID(3);
                director.Present<StateWaiting7K>();
            });
        });
    }

    auto showAllButton     = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_SHOW_ALL");
    auto waitingRoomButton = Load<Gx::Button>("STATE_ROOM/IDC_BUTTON_WAITING_ROOM");

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

void StateRoom::Update(double delta)
{
    Scene::Update(delta);
}

sf::RenderStates StateRoom::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    return Scene::Render(target, states);
}

void StateRoom::OnBackClicked()
{
    auto& director = GetDirector();
    director.Present<StatePlanet>();
}
