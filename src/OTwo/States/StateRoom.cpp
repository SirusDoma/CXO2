#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateBulletin.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/RoomContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <OTwo/UI/Common/Marquee.hpp>
#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Room/RoomList.hpp>
#include <OTwo/UI/Room/UserList.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>
#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>

#include <OTwo/Services/RoomService.hpp>
#include <OTwo/Services/CharacterService.hpp>
#include <OTwo/Services/WaitingService.hpp>

#include <OTwo/Messages/Requests/CreateRoomRequest.hpp>
#include <OTwo/Messages/Requests/JoinRoomRequest.hpp>

#include <OTwo/Messages/Responses/RoomListResponse.hpp>
#include <OTwo/Messages/Responses/UserListResponse.hpp>
#include <OTwo/Messages/Responses/CreateRoomResponse.hpp>
#include <OTwo/Messages/Responses/JoinRoomResponse.hpp>

#include <OTwo/Messages/Events/RoomCreatedEventData.hpp>
#include <OTwo/Messages/Events/RoomMusicChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomStateChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomTitleChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomUserCountChangedEventData.hpp>
#include <OTwo/Messages/Events/RoomRemovedEventData.hpp>

#include <OTwo/Models/Game.hpp>
#include <OTwo/Models/Room.hpp>

#include <OTwo/StringTable/Identifiers/Cache.hpp>
#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Room.hpp>
#include <OTwo/Utilities/StringFormatter.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/ToolTip.hpp>

using namespace StringTable::Identifiers;

StateRoom::StateRoom(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room,
    GameContext& game, RoomService& roomService, CharacterService& charService, ItemFactory& items) :
    m_mixer(mixer),
    m_charService(charService),
    m_service(roomService),
    m_session(session),
    m_room(room),
    m_game(game),
    m_items(items)
{
}

void StateRoom::Initialize()
{
    Initialize(RoomTransitionEventType::Normal);
}

void StateRoom::Initialize(const RoomTransitionEventType evType)
{
    State::Initialize();

    LoadCharacterInfo();
    LoadChannelInfo();

    const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_ROOM);
    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);

    const auto avatar = Instantiate<Avatar>(Resource::Room::IDC_AVATAR);
    avatar->SetVisible(false);

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
    channelNumber->SetValue(m_session.GetChannelID() + 1);

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

    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    roomList->SetEnterRoomCallback(std::bind(&StateRoom::JoinRoom, this, std::placeholders::_1));

    const auto createRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_CREATE_ROOM);
    createRoomButton->SetClickCallback(std::bind(&StateRoom::OnCreateRoomButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto quickJoinButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_QUICK_JOIN);
    quickJoinButton->SetClickCallback(std::bind(&StateRoom::OnQuickJoinRoomButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto showAllButton     = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_ALL);
    const auto waitingRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_WAITING);

    showAllButton->SetVisible(false);
    showAllButton->SetEnabled(false);
    waitingRoomButton->SetVisible(true);
    waitingRoomButton->SetEnabled(true);

    showAllButton->SetClickCallback(std::bind(&StateRoom::OnShowAllButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
    waitingRoomButton->SetClickCallback(std::bind(&StateRoom::OnWaitingButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto roomLeftButton  = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ROOM_LEFT);
    const auto roomRightButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ROOM_RIGHT);

    roomLeftButton->SetClickCallback(std::bind(&StateRoom::OnRoomLeftButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
    roomRightButton->SetClickCallback(std::bind(&StateRoom::OnRoomRightButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto musicShopButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_MUSIC_SHOP);
    musicShopButton->SetClickCallback(std::bind(&StateRoom::OnMusicShopButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto itemShopButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_ITEM_SHOP);
    itemShopButton->SetClickCallback(std::bind(&StateRoom::OnItemShopButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto myRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_MY_ROOM);
    myRoomButton->SetClickCallback(std::bind(&StateRoom::OnMyRoomButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto bulletinButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_BULLETIN);
    bulletinButton->SetClickCallback(std::bind(&StateRoom::OnBulletinButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto tutorialButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_TUTORIAL);
    tutorialButton->SetClickCallback(std::bind(&StateRoom::OnTutorialButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto optionButton = FindChild<Gx::Button>(Resource::Room::IDC_BUTTON_OPTION);
    optionButton->SetClickCallback(std::bind(&StateRoom::OnOptionButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    const auto backButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_BACK);
    backButton->SetClickCallback(std::bind(&StateRoom::OnBackButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

    if (evType == RoomTransitionEventType::Kick)
    {
        ShowDialog("You are forced out", DialogStyle::Information);
    }
}

void StateRoom::OnRoomCreated(const RoomCreatedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    roomList->Upsert(RoomInfo
    {
        /* .ID            = */ ev.ID,
        /* .State         = */ RoomState::Waiting,
        /* .Title         = */ ev.Title,
        /* .Locked        = */ ev.Locked,
        /* .MusicID       = */ 0,
        /* .Difficulty    = */ Difficulty::EX,
        /* .Mode          = */ ev.GameMode,
        /* .SpeedID       = */ 1,
        /* .Capacity      = */ 8,
        /* .UserCount     = */ 1,
        /* .MinLevelLimit = */ ev.MinLevelLimit,
        /* .MaxLevelLimit = */ ev.MaxLevelLimit
    });

    roomList->Invalidate();
}

void StateRoom::OnRoomMusicChanged(const RoomMusicChangedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    auto& room = roomList->GetRoom(ev.ID);

    room.MusicID    = ev.MusicID;
    room.Difficulty = ev.Difficulty;
    room.SpeedID    = ev.SpeedID;
    roomList->Invalidate();
}

void StateRoom::OnRoomStateChanged(const RoomStateChangedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    auto& room = roomList->GetRoom(ev.ID);

    room.State = ev.State;
    roomList->Invalidate();
}

void StateRoom::OnRoomTitleChanged(const RoomTitleChangedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    auto& room = roomList->GetRoom(ev.ID);

    room.Title = ev.Title;
    roomList->Invalidate();
}

void StateRoom::OnRoomUserCountChanged(const RoomUserCountChangedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    auto& room = roomList->GetRoom(ev.ID);

    room.UserCount = ev.UserCount;
    room.Capacity  = ev.Capacity;

    roomList->Invalidate();
}

void StateRoom::OnRoomRemoved(const RoomRemovedEventData& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    roomList->Remove(ev.ID);
}

void StateRoom::LoadCharacterInfo()
{
    auto callback = [this] (const auto& charInfo)
    {
        Invoke([this, charInfo]
        {
            const auto nicknameLabel = Instantiate<Gx::Label>(Resource::Room::IDC_TEXT_NICKNAME);
            nicknameLabel->SetString(fmt::format(L"Lv.{}: {}", charInfo.Level, charInfo.Name));

            const auto avatar = Instantiate<Avatar>(Resource::Room::IDC_AVATAR);
            avatar->SetGender(charInfo.Gender);

            for (auto [_, item] : m_items.GetDefaultItems(charInfo.Gender))
                avatar->SetDefaultItem(std::move(item));

            for (const auto id : charInfo.EquippedItemIDs)
                avatar->Equip(m_items.Create(id));

            avatar->SetVisible(true);
        });
    };

    if (m_session.GetCharacterInfo().Name.isEmpty())
    {
        m_charService.GetCharacterInfo([=] (const auto& charInfo)
        {
            m_session.SetCharacterInfo(charInfo);
            callback(charInfo);
        });
    }
    else
        callback(m_session.GetCharacterInfo());
}

void StateRoom::LoadChannelInfo()
{
    m_service.GetChannelInfo([this] (const auto& response)
    {
        Invoke([this, response]
        {

            const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
            roomList->Clear();

            for (const RoomInfo& room : response.Rooms)
            {
                if (room.State != RoomState::Unavailable)
                    roomList->Upsert(room);
            }

            roomList->Invalidate();
        });
    },
    [this] (const auto& response)
    {
        if (response.Users->empty())
            return;

        Invoke([this, users = response.Users.GetContainer()]
        {
            const auto userList  = Instantiate<UserList>(Resource::Room::IDC_USER_LIST);
            userList->Clear();

            for (const auto& user : users)
            {
                userList->AddUser(CharacterInfo
                {
                    user.Name,
                    Gender::Any,
                    Role::Normal,
                    user.Level
                });
            }

            userList->Invalidate();
        });
    },
    [this] (const auto& ex)
    {
        Invoke([this, ex]
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        });
    });
}

void StateRoom::CreateRoom(
    const sf::String& title,
    const GameMode mode,
    const std::string& password,
    const unsigned int minLevelLimit,
    const unsigned int maxLevelLimit
)
{
    m_busy = true;

    const auto& musicList = m_session.GetInstalledMusic();
    if (musicList.empty())
    {
        ShowDialog("You have no tune", DialogStyle::Information, false, [=] (bool)
        {
            m_busy = false;
        });

        return;
    }

    const auto& music = musicList[0];
    const auto request = CreateRoomRequest
    {
        title,
        mode,
        !password.empty(),
        password.empty() ? std::string("1") : password,
        static_cast<std::uint8_t>(minLevelLimit),
        static_cast<std::uint8_t>(maxLevelLimit)
    };

    m_service.CreateRoom(
        request,
        [=] (const CreateRoomResponse& response)
        {
            Invoke([=]
            {
                if (response.ResultCode != CreateRoomResult::Success)
                {
                    ShowDialog("Channel is full.", DialogStyle::Information);
                    return;
                }

                const auto room = RoomInfo
                {
                    /* .ID            = */ response.ID,
                    /* .State         = */ RoomState::Waiting,
                    /* .Title         = */ title,
                    /* .Locked        = */ !password.empty(),
                    /* .MusicID       = */ music.ID,
                    /* .Difficulty    = */ Difficulty::EX,
                    /* .Mode          = */ mode,
                    /* .SpeedID       = */ 0,
                    /* .Capacity      = */ 8,
                    /* .UserCount     = */ 1,
                    /* .MinLevelLimit = */ static_cast<std::uint8_t>(minLevelLimit),
                    /* .MaxLevelLimit = */ static_cast<std::uint8_t>(maxLevelLimit)
                };


                m_room.UpdateFrom(room);
                m_room.SetMapID(MapInfo::RandomID);
                m_room.SetRandomizedMapID(0);

                auto& master = m_room.GetSlot(0);
                master = RoomSlot
                {
                    /* .Member   = */ m_session.GetCharacterInfo(),
                    /* .State    = */ RoomSlotState::Occupied,
                    /* .IsMaster = */ true,
                    /* .Ready    = */ true,
                    /* .Team     = */ RoomTeam::A
                };

                GetDirector().Present<StateWaiting7K>();
            });
        },
        [=] (const auto& ex)
        {
            Invoke([=]
            {
                ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
                {
                    GetDirector().Dismiss<StatePlanet>();
                });
            });
        }
    );
}

void StateRoom::JoinRoom(const RoomInfo& room)
{
    if (m_busy)
        return;

    if (room.Mode == GameMode::Single)
    {
        ShowDialog("As it is a single room, any user cannot enter.", DialogStyle::Information);
        return;
    }

    if (room.State != RoomState::Waiting)
    {
        ShowDialog("The game is in process.", DialogStyle::Information);
        return;
    }

    if (room.UserCount >= room.Capacity)
    {
        ShowDialog("The room is filled.", DialogStyle::Information);
        return;
    }

    const auto& musicList = m_session.GetInstalledMusic();
    if (room.MusicID <= std::numeric_limits<std::uint16_t>::max())
    {
        const auto it = std::find_if(musicList.begin(), musicList.end(), [musicID = room.MusicID] (const auto& header)
        {
            return header.ID == musicID;
        });

        if (it == musicList.end())
        {
            ShowDialog(fmt::format(L"Please download the following tune. \n\n<< file : o2ma{} >>", room.MusicID), DialogStyle::Information);
            return;
        }
    }
    else
    {
        const std::uint8_t randomBit = static_cast<std::uint8_t>((room.MusicID >> 28) & 0xFF);
        constexpr int randomMaxBit = static_cast<int>(LevelCategory::Level1) |
                                     static_cast<int>(LevelCategory::Level2) |
                                     static_cast<int>(LevelCategory::Level3) |
                                     static_cast<int>(LevelCategory::Level4);

        if (randomBit == 0 || randomBit > randomMaxBit)
        {
            ShowDialog(fmt::format(L"Please download the following tune. \n\n<< file : o2ma{} >>", room.MusicID), DialogStyle::Information);
            return;
        }
    }

    auto callback = [=] (const std::string& password)
    {
        m_busy = true;
        m_service.JoinRoom(
            JoinRoomRequest{room.ID, password},
            [=](const JoinRoomResponse& response)
            {
                if (response.Result != JoinResult::Success)
                {
                    if (response.Result == JoinResult::Full)
                        ShowDialog("The room is filled.", DialogStyle::Information);
                    else if (response.Result == JoinResult::InvalidPassword)
                        ShowDialog("Wrong password", DialogStyle::Information);
                    else if (response.Result == JoinResult::InvalidMode)
                        ShowDialog("The room is single playmode", DialogStyle::Information);
                    else if (response.Result == JoinResult::InProgress)
                        ShowDialog("The game already has started.", DialogStyle::Information);
                    else
                        ShowDialog("[Fail] Please contact administrator when this message shows.",
                                   DialogStyle::Information);

                    m_busy = false;
                    return;
                }

                std::uint32_t musicID = response.MusicID;
                auto random = static_cast<LevelCategory>(0);
                if (musicID >= std::numeric_limits<std::uint16_t>::max())
                {
                    const std::uint8_t randomBit = static_cast<std::uint8_t>((room.MusicID >> 28) & 0xFF);
                    constexpr int randomMaxBit = static_cast<int>(LevelCategory::Level1) |
                                                 static_cast<int>(LevelCategory::Level2) |
                                                 static_cast<int>(LevelCategory::Level3) |
                                                 static_cast<int>(LevelCategory::Level4);

                    if (randomBit >= 1 && randomBit <= randomMaxBit)
                    {
                        random  = static_cast<LevelCategory>(randomBit);
                        musicID = response.MusicID & 0xFF;
                    }
                }

                m_room.UpdateFrom(RoomInfo
                {
                    /* .ID            = */ room.ID,
                    /* .State         = */ RoomState::Waiting,
                    /* .Title         = */ response.Title,
                    /* .Locked        = */ !password.empty(),
                    /* .MusicID       = */ musicID,
                    /* .Difficulty    = */ response.Difficulty,
                    /* .Mode          = */ response.Mode,
                    /* .SpeedID       = */ response.SpeedID,
                    /* .Capacity      = */ 8,
                    /* .UserCount     = */ 1,
                    /* .MinLevelLimit = */ room.MinLevelLimit,
                    /* .MaxLevelLimit = */ room.MaxLevelLimit
                });

                m_room.SetMapID(response.Map.GetMapID());
                m_room.SetRandomizedMapID(response.Map.GetRandomizedMap());

                m_room.SetRandomLevel(random);
                for (const auto& source: response.Slots)
                {
                    auto& slot = m_room.GetSlot(source.Index);
                    slot.State = source.State;

                    if (slot.State == RoomSlotState::Unoccupied || slot.State == RoomSlotState::Locked)
                    {
                        slot.Member = std::nullopt;
                        continue;
                    }

                    slot.IsMaster = source.Member.IsRoomMaster;
                    slot.Team = source.Member.Team;
                    slot.Ready = source.Member.IsRoomMaster || source.Member.Ready;
                    slot.Member = CharacterInfo
                    {
                        /* .Name            = */ source.Member.Name,
                        /* .Gender          = */ source.Member.Gender,
                        /* .Role            = */ Role::Normal,
                        /* .Level           = */ source.Member.Level,
                        /* .Experience      = */ 0,
                        /* .RankStats       = */ {},
                        /* .Wallet          = */ {},
                        /* .EquippedItemIDs = */ source.Member.EquippedItemIDs,
                        /* .Inventory       = */ {},
                        /* .MusicIDs        = */ source.Member.MusicIDs
                    };
                }

                GetDirector().Present<StateWaiting7K>();
            }, [=](const auto& ex)
            {
                Invoke([=]
                {
                    ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=](bool)
                    {
                        GetDirector().Dismiss<StatePlanet>();
                    });
                });
            }
        );
    };

    if (room.Locked)
    {
        const auto passwordDialog = Instantiate<Gx::Dialog>(Resource::Room::IDC_DIALOG_PASSWORD);
        const auto field = passwordDialog->FindChild<Gx::InputField>(Resource::Room::Password::IDC_EDIT_PASSWORD);
        field->SetMaximumTextLength(20);
        field->SetMasked(true);
        field->SetString(std::string());

        passwordDialog->SetAcceptCallback([=]
        {
            callback(field->GetString());
        });

        auto ctx = Gx::DialogPresentationContext();
        ctx.Bounds = sf::FloatRect{{0, 0}, GetView().getSize()};
        ctx.Prompt = "Please input room password";

        Present(*passwordDialog, ctx);
    }
    else
        callback("1");
}

void StateRoom::OnCreateRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto createRoomDialog = Instantiate<CreateRoomDialog>(Resource::Room::IDC_DIALOG_CREATE_ROOM);

    if (m_busy)
        return;

    const auto sfxAccept   = Instantiate<sf::Sound>(Sound::Effects::EF_02);
    m_mixer.Play(*sfxAccept, Sound::Channel::SFX);

    Present(*createRoomDialog, Gx::PresentationContext::Default);
    createRoomDialog->SetAcceptCallback([=]
    {
        CreateRoom(
            createRoomDialog->GetRoomName(),
            createRoomDialog->GetRoomMode(),
            createRoomDialog->GetRoomPassword(),
            createRoomDialog->GetMinLevelLimit(),
            createRoomDialog->GetMaxLevelLimit()
        );
    });
}

void StateRoom::OnQuickJoinRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto roomList = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    const auto room = roomList->GetWaitingRoom();

    if (!room)
    {
        const auto tooltip = Instantiate<Gx::ToolTip>(Resource::Room::IDC_TOOLTIP_QUICK_JOIN);
        tooltip->SetString("No available room.");
        tooltip->Show();

        return;
    }

    JoinRoom(*room);
}

void StateRoom::OnShowAllButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto sfxToggle         = Instantiate<sf::Sound>(Sound::Effects::EF_14);
    const auto roomList          = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    const auto showAllButton     = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_ALL);
    const auto waitingRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_WAITING);

    m_mixer.Play(*sfxToggle, Sound::Channel::SFX);

    showAllButton->SetVisible(false);
    showAllButton->SetEnabled(false);

    waitingRoomButton->SetVisible(true);
    waitingRoomButton->SetEnabled(true);

    roomList->ShowAll();
}

void StateRoom::OnWaitingButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto sfxToggle         = Instantiate<sf::Sound>(Sound::Effects::EF_14);
    const auto roomList          = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);
    const auto showAllButton     = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_ALL);
    const auto waitingRoomButton = Instantiate<Gx::Button>(Resource::Room::IDC_BUTTON_SHOW_WAITING);

    m_mixer.Play(*sfxToggle, Sound::Channel::SFX);

    showAllButton->SetVisible(true);
    showAllButton->SetEnabled(true);

    waitingRoomButton->SetVisible(false);
    waitingRoomButton->SetEnabled(false);

    roomList->ShowWaitingOnly();
}

void StateRoom::OnRoomLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto sfxNavigate = Instantiate<sf::Sound>(Sound::Effects::EF_07);
    const auto roomList    = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);

    m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
    roomList->PreviousPage();
}

void StateRoom::OnRoomRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto sfxNavigate = Instantiate<sf::Sound>(Sound::Effects::EF_07);
    const auto roomList    = Instantiate<RoomList>(Resource::Room::IDC_ROOM_LIST);

    m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
    roomList->NextPage();
}

void StateRoom::OnMusicShopButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
{
    auto& director = GetDirector();
    director.Present<StateMusicShop>();
}

void StateRoom::OnItemShopButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
{
    auto& director = GetDirector();
    director.Present<StateItemShop>();
}

void StateRoom::OnMyRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
{
    auto& director = GetDirector();
    director.Present<StateMyRoom>();
}

void StateRoom::OnBulletinButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
{
    auto& director = GetDirector();
    director.Present<StateBulletin>();
}

void StateRoom::OnOptionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    const auto optionDialog = Instantiate<OptionDialog>(Resource::Room::IDC_DIALOG_OPTION);
    Present(*optionDialog, Gx::PresentationContext::Default);
}

void StateRoom::OnTutorialButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    auto chart    = std::make_unique<Chart>();
    chart->Source = "Tutorial.ojn";

    auto& resources = GetResources(ResourceScope::Shared);
    if (const auto metadata = O2JamChartMetadataLoader().LoadFromFile(chart->Source, Gx::ResourceContext::Default))
    {
        if (auto image = O2JamChartLoader::LoadCoverArt(*metadata, Gx::ResourceContext::Default); image)
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

void StateRoom::OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
{
    if (m_busy)
        return;

    m_service.ChannelLogout([this]
    {
        auto& director = GetDirector();
        if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_35))
            m_mixer.Play(*sfx);

        director.Dismiss<StatePlanet>();
    },
    [=] (const auto& ex)
    {
        Invoke([=]
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        });
    });
}
