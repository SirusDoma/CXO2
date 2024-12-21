#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateBulletin.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

#include <OTwo/UI/Common/Marquee.hpp>
#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Room/RoomList.hpp>
#include <OTwo/UI/Room/UserList.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>
#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/MusicSelectionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Models/Room.hpp>

#include <OTwo/StringTable/Identifiers/Cache.hpp>
#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Room.hpp>
#include <OTwo/Utilities/StringFormatter.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>

using namespace StringTable::Identifiers;

StateRoom::StateRoom(Gx::AudioMixer& mixer, SessionContext& session, MusicSelectionContext& selection, GameContext& game, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_selection(selection),
    m_game(game),
    m_items(items)
{
}

void StateRoom::Initialize()
{
    State::Initialize();

    auto& director         = GetDirector();
    const auto bgm         = Instantiate<sf::Music>(Sound::BGM::BG_ROOM);
    const auto sfxAccept   = Instantiate<sf::Sound>(Sound::Effects::EF_02);
    const auto sfxNavigate = Instantiate<sf::Sound>(Sound::Effects::EF_07);
    const auto sfxToggle   = Instantiate<sf::Sound>(Sound::Effects::EF_14);

    const auto player = m_session.GetCurrentPlayer();
    const auto nicknameLabel = Instantiate<Gx::Label>(Resource::Room::IDC_TEXT_NICKNAME);
    nicknameLabel->SetString(fmt::format(L"Lv.{}: {}", player.Level, player.Name));

    const auto avatar = Instantiate<Avatar>(Resource::Room::IDC_AVATAR);
    avatar->SetGender(player.Gender);
    for (auto [_, item] : m_items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(std::move(item));

    for (const auto id : player.EquippedItemIDs)
        avatar->Equip(m_items.Create(id));

    const auto notice = Instantiate<Marquee>(Resource::Room::IDC_TEXT_NOTICE);
    notice->SetString("Welcome to O2Jam! Let's play together~");

    const auto channelCategory = Instantiate<Gx::Image>(Resource::Room::IDC_IMAGE_CHANNEL_CATEGORY);
    switch (m_session.GetMusicHall())
    {
        case MusicHall::Kalliope: channelCategory->SetFrame("Kalliope"); break;
        case MusicHall::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case MusicHall::Philix:   channelCategory->SetFrame("Philix");   break;
        case MusicHall::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case MusicHall::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case MusicHall::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }

    const auto channelNumber = Instantiate<Gx::BitmapNumber>(Resource::Room::IDC_NUMBER_CHANNEL_ID);
    channelNumber->SetValue(m_session.GetChannelID());

    const auto chatPanel = Instantiate<ChatPanel>(Resource::Room::IDC_CHAT_PANEL);
    chatPanel->SetMaximumTextLength(50);

    const auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam");
    chatWindow->PushSystemMessage("Alt+Enter     : Toggle windowed/fullscreen mode setting");
    chatWindow->PushSystemMessage("/w Receiver  : Send message (whisper)");
    chatWindow->PushSystemMessage("F7                 : Effect 2D/3D mode setting");
    chatWindow->PushSystemMessage("F8                 : Cursor mode setting");
    chatWindow->PushSystemMessage("F9                 : Toggle equalizer on/off");
    chatWindow->PushSystemMessage("F10               : Toggle Vsync on/off");

    const auto userList = Instantiate<UserList>(Resource::Room::IDC_USER_LIST);
    auto users = std::vector<Player>();
    userList->AddUser(m_session.GetCurrentPlayer());

    for (unsigned int i = 0; i < 34; i++)
        userList->AddUser(Player{i + 3, Role::Normal, fmt::format("Dummy {}", i), static_cast<signed short>(i) });

    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
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
        roomList->Add(room);

    const auto createRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_CREATE_ROOM);
    if (const auto createRoomDialog = Instantiate<CreateRoomDialog>(Resource::Room::IDC_DIALOG_CREATE_ROOM); createRoomDialog)
    {
        createRoomButton->SetClickCallback([=, &director] (auto& sender, auto& ev) {
            m_mixer.Play(*sfxAccept, Sound::Channel::SFX);
            Present(*createRoomDialog);
            createRoomDialog->SetAcceptCallback([&] () {
                const auto musicList = m_session.GetInstalledMusic();
                const auto music = !m_selection.GetMetadata().Source.empty() ? m_selection.GetMetadata() : musicList[musicList.size() - 1];
                m_session.SetCurrentRoom(Room{
                    4,
                    m_session.GetCurrentPlayer().ID,
                    createRoomDialog->GetRoomName(),
                    music.ToChartMetadataView(m_selection.GetDifficulty()),
                    m_selection.GetDifficulty(),
                    createRoomDialog->GetRoomMode(),
                    SongMode::Normal,
                    RoomState::Waiting,
                    m_selection.GetSpeed(),
                    !createRoomDialog->GetRoomPassword().empty(),
                    8,
                    createRoomDialog->GetMinLevelLimit(),
                    createRoomDialog->GetMaxLevelLimit(),
                    {
                        RoomMember{m_session.GetCurrentPlayer(), RoomTeam::A, sf::Color::Transparent, 0, true},
                        {},
                        {},
                        RoomMember{Player{2, Role::Normal,"Random #1", 82, 0, 0, 0, 0, Gender::Male, 0, 0, 0, 0, 0, {221, 304}}, RoomTeam::F, sf::Color::Transparent, 0, true},
                        {},
                        {},
                        {},
                        RoomMember{Player{3, Role::Normal, "Random #2", 79, 0, 0, 0, 0, Gender::Male, 0, 0, 0, 0, 0, {39}}, RoomTeam::G, sf::Color::Transparent, 0, false}
                    }
                });
                director.Present<StateWaiting7K>();
            });
        });
    }

    const auto showAllButton     = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_ALL);
    const auto waitingRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_WAITING);

    showAllButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxToggle, Sound::Channel::SFX);

        showAllButton->SetVisible(false);
        showAllButton->SetEnabled(false);

        waitingRoomButton->SetVisible(true);
        waitingRoomButton->SetEnabled(true);

        roomList->ShowWaitingOnly();
    });

    waitingRoomButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxToggle, Sound::Channel::SFX);

        showAllButton->SetVisible(true);
        showAllButton->SetEnabled(true);

        waitingRoomButton->SetVisible(false);
        waitingRoomButton->SetEnabled(false);

        roomList->ShowAll();
    });

    const auto roomLeftButton  = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ROOM_LEFT);
    const auto roomRightButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ROOM_RIGHT);

    roomLeftButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
        roomList->PreviousPage();
    });

    roomRightButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
        roomList->NextPage();
    });

    const auto musicShopButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_MUSIC_SHOP);
    musicShopButton->SetClickCallback([this](auto& , auto& ) { OnMusicShopButtonClicked(); });

    const auto itemShopButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ITEM_SHOP);
    itemShopButton->SetClickCallback([this](auto& , auto& ) { OnItemShopButtonClicked(); });

    const auto myRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_MY_ROOM);
    myRoomButton->SetClickCallback([this](auto& , auto& ) { OnMyRoomButtonClicked(); });

    const auto bulletinButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_BULLETIN);
    bulletinButton->SetClickCallback([this](auto& , auto& ) { OnBulletinButtonClicked(); });

    const auto tutorialButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_TUTORIAL);
    tutorialButton->SetClickCallback([this](auto& , auto& ) { OnTutorialButtonClicked(); });

    if (const auto optionDialog = Instantiate<OptionDialog>(Resource::Room::IDC_DIALOG_OPTION); optionDialog)
    {
        const auto optionButton = FindChild<Gx::Button>(Resource::Room::IDC_BUTTON_OPTION);
        optionButton->SetClickCallback([=] (auto& sender, auto& ev) {
            Present(*optionDialog);
        });
    }

    const auto backButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_BACK);
    backButton->SetClickCallback([&](auto& , auto& ) { OnBackButtonClicked(); });

    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);
}

void StateRoom::OnKeyPressed(const sf::Event::KeyPressed& ev)
{
    State::OnKeyPressed(ev);
}

void StateRoom::OnMusicShopButtonClicked() const
{
    auto& director = GetDirector();
    director.Present<StateMusicShop>();
}

void StateRoom::OnItemShopButtonClicked() const
{
    auto& director = GetDirector();
    director.Present<StateItemShop>();
}

void StateRoom::OnMyRoomButtonClicked() const
{
    auto& director = GetDirector();
    director.Present<StateMyRoom>();
}

void StateRoom::OnBulletinButtonClicked() const
{
    auto& director = GetDirector();
    director.Present<StateBulletin>();
}

void StateRoom::OnTutorialButtonClicked()
{
    auto chart    = std::make_unique<Chart>();
    chart->Source = "Tutorial.ojn";

    auto& resources = GetResources(ResourceScope::Shared);
    if (const auto metadata = ChartMetadataLoader().LoadFromFile(chart->Source, Gx::ResourceContext::Default))
    {
        if (auto image = ChartLoader::LoadCoverArt(*metadata, Gx::ResourceContext::Default); image)
            resources.Store<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER, std::move(image), Gx::CacheMode::Update);
        else
            resources.Destroy<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER);
    }

    m_game.GetConfig().KeyBindings[KeyMode::Seven] = GameConfig().KeyBindings[KeyMode::Seven];
    m_game.SetChart(std::move(chart));
    m_game.SetMode(GameMode::Tutorial);
    m_game.SetDifficulty(Difficulty::EX);
    m_game.SetSpeed(1.0);
    m_game.SetMapID(1);
    m_game.SetEffectID(1);

    auto& director = GetDirector();
    director.Present<StateLoading>();
}

void StateRoom::OnBackButtonClicked() const
{
    auto& director = GetDirector();
    director.Dismiss<StatePlanet>();
}
