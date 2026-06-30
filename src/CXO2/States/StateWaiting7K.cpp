#include <CXO2/States/StateWaiting7K.hpp>

#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StatePlaying7K.hpp>

#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>

#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Services/WaitingService.hpp>

#include <CXO2/Messages/RoomInfo.hpp>
#include <CXO2/Messages/Requests/UpdateMapRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Messages/Events/StartGameEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberJoinedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberLeftEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberTeamChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingSlotChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMapChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingTitleChangedEventData.hpp>

#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/Waiting7K.hpp>
#include <CXO2/StringTable/Identifiers/Cache.hpp>
#include <CXO2/StringTable/Identifiers/Map.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/UI/Waiting/MapSelector.hpp>
#include <CXO2/UI/Waiting/InstrumentSelector.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>
#include <CXO2/Services/MessagingService.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    namespace
    {
        std::string GetRoomLevelCode(const RoomContext& room, const bool useNormalMode = false)
        {
            std::string speedStr(4, '\0');
            if (room.GetSpeed() > 0)
            {
                if (std::fmod(room.GetSpeed(), 1.0f) != 0)
                    speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", room.GetSpeed()));
                else
                    speedStr = std::to_string(static_cast<int>(room.GetSpeed()));
            }
            else
                speedStr = "R";

            if (useNormalMode || static_cast<int>(room.GetRandomLevel()) == 0)
            {
                std::string diffName;
                switch (room.GetDifficulty())
                {
                    case Difficulty::EX: diffName = "EX"; break;
                    case Difficulty::NX: diffName = "NX"; break;
                    case Difficulty::HX: diffName = "HX"; break;
                    case Difficulty::MX: diffName = "MX"; break;
                }

                return diffName + speedStr;
            }

            if (static_cast<int>(room.GetRandomLevel()) != 0)
            {
                return "RX" + speedStr;
            }

            return "MX" + speedStr;
        }

        std::pair<int, int> GetRandomLevelRange(LevelCategory randomLevel)
        {
            const auto levels = static_cast<int>(randomLevel);

            int start = 0;
            int end   = 0;

            if (levels & static_cast<int>(LevelCategory::Level1))
            {
                start = 1;
                end = 5;
            }

            if (levels & static_cast<int>(LevelCategory::Level2))
            {
                if (start == 0)
                    start = 5;

                end = 9;
            }

            if (levels & static_cast<int>(LevelCategory::Level3))
            {
                if (start == 0)
                    start = 9;

                end = 13;
            }

            if (levels & static_cast<int>(LevelCategory::Level4))
            {
                if (start == 0)
                    start = 13;

                end = 0;
            }

            return std::pair(start, end);
        }
    }

    StateWaiting7K::StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& game, WaitingService& service, MessagingService& messaging, ItemFactory& items) :
        m_mixer(mixer),
        m_session(session),
        m_room(room),
        m_game(game),
        m_service(service),
        m_messaging(messaging),
        m_items(items)
    {
    }

    void StateWaiting7K::Initialize()
    {
        State::Initialize();

        const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_WAITING);

        const auto channelCategory = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_CHANNEL_CATEGORY);
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

        InitializeAvatars();

        // Team buttons
        const auto teamButtons = Instantiate<Gx::UiContainer>(Resource::Waiting7K::IDC_CONTAINER_TEAM_BUTTONS);
        auto teamButtonMatcher = [=] (const RoomTeam team) -> Gx::RadioButton*
        {
            switch (team)
            {
                default:
                case RoomTeam::A: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_A);
                case RoomTeam::B: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_B);
                case RoomTeam::C: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_C);
                case RoomTeam::D: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_D);
                case RoomTeam::E: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_E);
                case RoomTeam::F: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_F);
                case RoomTeam::G: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_G);
                case RoomTeam::H: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_H);
            }
        };

        if (m_slot)
        {
            if (const auto currentTeamButton = teamButtonMatcher(m_slot->Team); currentTeamButton)
                currentTeamButton->SetCheckedState(true);
        }

        for (const auto team : { RoomTeam::A, RoomTeam::B, RoomTeam::C, RoomTeam::D, RoomTeam::E, RoomTeam::F, RoomTeam::G, RoomTeam::H })
        {
            const auto teamButton = teamButtonMatcher(team);
            if (!teamButton)
                continue;

            teamButton->SetCheckStateChangeCallback([=] (auto& sender) { OnTeamButtonStateChanged(sender, team); });
        }

        // Emoticon dialog
        const auto emoticonDialog      = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_EMOTICON);
        const auto emoticonPrevButton  = emoticonDialog->FindChild<Gx::Button>(Resource::Waiting7K::Emoticon::IDC_BUTTON_LEFT);
        const auto emoticonNextButton  = emoticonDialog->FindChild<Gx::Button>(Resource::Waiting7K::Emoticon::IDC_BUTTON_RIGHT);

        emoticonPrevButton->SetClickCallback(std::bind(&StateWaiting7K::OnEmoticonPreviousPageButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
        emoticonNextButton->SetClickCallback(std::bind(&StateWaiting7K::OnEmoticonNextPageButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

        const auto emoticonHelpButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_EMOTICON);
        emoticonHelpButton->SetClickCallback(std::bind(&StateWaiting7K::OnEmoticonButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

        // Change title dialog
        const auto changeTitleDialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto changeTitleBox = changeTitleDialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);
        changeTitleBox->SetMaximumTextLength(21);
        changeTitleDialog->SetAcceptCallback(std::bind(&StateWaiting7K::OnChangeTitleDialogAccepted, this));

        const auto changeTitleButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_CHANGE_TITLE);
        changeTitleButton->SetClickCallback(std::bind(&StateWaiting7K::OnChangeTitleButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

        // Map selector
        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);
        mapSelector->SetMapChangedCallback(std::bind(&StateWaiting7K::OnMapSelectorStateChanged, this, std::placeholders::_1));
        mapSelector->SetEffectChangedCallback([=] (const unsigned int effectID){ m_room.SetEffectID(effectID); });

        mapSelector->SetMapID(m_room.GetMapID(), true);
        mapSelector->SetEffectID(m_room.GetEffectID());
        mapSelector->SetControlsEnabled(m_room.GetCurrentSlot().IsMaster);

        // Instrument selector
        const auto instrumentSelector = Instantiate<InstrumentSelector>(Resource::Waiting7K::IDC_CONTAINER_INSTRUMENT_SELECTOR);
        instrumentSelector->SetInstrumentSelectCallback(std::bind(&StateWaiting7K::OnInstrumentSelectorStateChanged, this, std::placeholders::_1));

        for (const auto id : m_session.GetCharacterInfo().Inventory)
            instrumentSelector->AddInstrumentMetadata(m_items.GetItemMetadata(id));

        for (const auto id : m_session.GetCharacterInfo().EquippedItemIDs)
            instrumentSelector->AddInstrumentMetadata(m_items.GetItemMetadata(id));

        // Select music dialog
        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        selectMusicDialog->SetAcceptCallback(std::bind(&StateWaiting7K::OnSelectMusicDialogAccepted, this));

        const auto selectMusicButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_SELECT_MUSIC);
        selectMusicButton->SetClickCallback(std::bind(&StateWaiting7K::OnSelectMusicButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

        // Chat panel & window
        const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        chatPanel->SetMaximumTextLength(50);

        const auto chatWindow = chatPanel->GetChatWindow();
        chatWindow->PushSystemMessage("Welcome to O2Jam!");
        chatWindow->PushSystemMessage("Let's play together~");

        if (const auto cover = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_COVER_MUSIC))
            cover->SetVisible(!m_room.GetCurrentSlot().IsMaster);

        // Buttons
        const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START);
        startButton->SetVisible(m_room.GetCurrentSlot().IsMaster);
        startButton->SetEnabled(startButton->IsVisible());
        startButton->SetCheckStateChangeCallback(std::bind(&StateWaiting7K::OnStartStateChanged, this, std::placeholders::_1));

        const auto readyButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_READY);
        readyButton->SetVisible(!m_room.GetCurrentSlot().IsMaster);
        readyButton->SetEnabled(readyButton->IsVisible());
        readyButton->SetCheckStateChangeCallback(std::bind(&StateWaiting7K::OnReadyStateChanged, this, std::placeholders::_1));

        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);
        backButton->SetClickCallback(std::bind(&StateWaiting7K::OnBackButtonClicked, this, std::placeholders::_1, std::placeholders::_2));

        InvalidateRoomInfo();
        InvalidateMembers();

        if (m_room.GetCurrentSlot().IsMaster && static_cast<int>(m_room.GetRandomLevel()) != 0)
        {
            selectMusicDialog->Initialize();
            OnSelectMusicDialogAccepted();
        }

        if (m_room.GetCurrentSlot().IsMaster && (m_room.GetMapID() == MapInfo::RandomID || m_room.GetMapID() == 0))
            OnMapSelectorStateChanged(0);

        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
    }

    void StateWaiting7K::OnSlotChanged(const WaitingSlotChangedEventData& ev)
    {
        if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
            return;

        auto& slot = m_room.GetSlot(ev.ID);
        auto oldSlot = slot;
        switch (ev.Type)
        {
            case RoomSlotEventType::Unlock:
            {
                assert(!slot.Member.has_value() && "Slot is not empty");

                slot.State = RoomSlotState::Unoccupied;
                slot.Member = std::nullopt;

                InvalidateMembers();
                break;
            }
            case RoomSlotEventType::Lock:
            {
                assert(!slot.Member.has_value() && "Slot is not empty");

                slot.State = RoomSlotState::Locked;
                slot.Member = std::nullopt;

                InvalidateMembers();
                break;
            }
            case RoomSlotEventType::Kicked:
            {
                assert(slot.Member.has_value() && "Slot is empty");
                slot.State = RoomSlotState::Unoccupied;

                auto name = slot.Member->Name;
                slot.Member = std::nullopt;

                const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
                const auto chatWindow = chatPanel->GetChatWindow();
                chatWindow->PushSystemMessage(fmt::format(L"[INFO] {} forced out", name));

                InvalidateMembers();
                break;
            }
        }
    }

    void StateWaiting7K::OnMemberJoined(const WaitingMemberJoinedEventData& ev)
    {
        if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
            return;

        auto& slot    = m_room.GetSlot(ev.ID);
        slot.State    = RoomSlotState::Occupied;
        slot.Ready    = ev.Ready;
        slot.Team     = ev.Team;
        slot.IsMaster = false;
        slot.Member   = CharacterInfo
        {
            /* .Name            = */ ev.Name,
            /* .Gender          = */ ev.Gender,
            /* .Role            = */ Role::Normal,
            /* .Level           = */ ev.Level,
            /* .Experience      = */ 0,
            /* .RankStats       = */ {},
            /* .Wallet          = */ {},
            /* .EquippedItemIDs = */ ev.EquippedItemIDs,
            /* .Inventory       = */ {},
            /* .MusicIDs        = */ ev.MusicIDs
        };

        const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        const auto chatWindow = chatPanel->GetChatWindow();

        chatWindow->PushSystemMessage(fmt::format(L"[INFO] {} just came in.", slot.Member->Name));
        InvalidateMembers();

        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        selectMusicDialog->Initialize();
    }

    void StateWaiting7K::OnMemberLeft(const WaitingMemberLeftEventData& ev)
    {
        if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
            return;

        auto& slot = m_room.GetSlot(ev.ID);
        auto name = slot.Member->Name;

        if (slot.IsMaster)
        {
            if (ev.RoomMasterID < 0 || ev.RoomMasterID >= RoomContext::MaxCapacity)
                return;

            auto& newMaster    = m_room.GetSlot(ev.RoomMasterID);
            newMaster.IsMaster = true;
            newMaster.Ready    = true;
        }

        slot.State    = RoomSlotState::Unoccupied;
        slot.IsMaster = false;
        slot.Ready    = false;
        slot.Member   = std::nullopt;

        const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        const auto chatWindow = chatPanel->GetChatWindow();
        chatWindow->PushSystemMessage(fmt::format(L"[INFO] {} has left", name));

        InvalidateMembers();
        InvalidateRoomInfo();

        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        selectMusicDialog->Initialize();
    }

    void StateWaiting7K::OnMemberTeamChanged(const WaitingMemberTeamChangedEventData& ev)
    {
        if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
            return;

        auto& slot = m_room.GetSlot(ev.ID);
        slot.Team  = ev.Team;

        InvalidateAvatarInfo();
    }

    void StateWaiting7K::OnMemberReadyStateChanged(const WaitingMemberReadyStateChangedEventData& ev)
    {
        if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
            return;

        auto& slot = m_room.GetSlot(ev.ID);
        slot.Ready = ev.Ready;

        InvalidateAvatarInfo();
    }

    void StateWaiting7K::OnMemberEmoticon(const CharacterInfo& sender, const sf::String& chatData)
    {
        auto code = std::string();
        auto text = chatData.toAnsiString();
        auto data = std::vector<std::uint8_t>(text.begin(), text.end());

        if (data == std::vector<std::uint8_t>{ 47, 33 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK;
        else if (data == std::vector<std::uint8_t>{ 47, 63 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_QUESTION;
        else if (data == std::vector<std::uint8_t>{ 47, 187, 231, 182, 251, 199, 216 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HEART;
        else if (data == std::vector<std::uint8_t>{ 47, 51, 50, 49 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_COUNT;
        else if (data == std::vector<std::uint8_t>{ 47, 126 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_NOTE;
        else if (data == std::vector<std::uint8_t>{ 47, 33, 33 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK2;
        else if (data == std::vector<std::uint8_t>{ 47, 185, 204, 191, 246 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BROKEN_HEART;
        else if (data == std::vector<std::uint8_t>{ 47, 55 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_CLOVER;
        else if (data == std::vector<std::uint8_t>{ 47, 178, 201 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_FLOWER;
        else if (data == std::vector<std::uint8_t>{ 47, 185, 221, 194, 166 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_IDEA;
        else if (data == std::vector<std::uint8_t>{ 47, 190, 200, 179, 231 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HI;
        else if (data == std::vector<std::uint8_t>{ 47, 176, 237 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_GO;
        else if (data == std::vector<std::uint8_t>{ 47, 183, 185, 181, 240 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_READY;
        else if (data == std::vector<std::uint8_t>{ 47, 193, 193, 190, 198 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_APPROVAL;
        else if (data == std::vector<std::uint8_t>{ 47, 189, 200, 190, 238 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OBJECTION;
        else if (data == std::vector<std::uint8_t>{ 47, 200, 229, 200, 229 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_SMILE;
        else if (data == std::vector<std::uint8_t>{ 47, 199, 207, 199, 207 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_HAHA;
        else if (data == std::vector<std::uint8_t>{ 47, 197, 169, 197, 169 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_VICTORY;
        else if (data == std::vector<std::uint8_t>{ 47, 192, 185 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_LOSE;
        else if (data == std::vector<std::uint8_t>{ 47, 46, 46, 46 })
            code = Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BY_DEGREES;

        const auto avatarList = Instantiate<Gx::List>(Resource::Waiting7K::IDC_LIST_AVATAR);

        int memberID = 0;
        Avatar* avatar = nullptr;
        for (const auto child : avatarList->GetChildren())
        {
            if (memberID >= RoomContext::MaxCapacity)
                break;

            const auto container = dynamic_cast<Gx::UiContainer*>(child);
            if (!container)
                continue;

            const auto& slot = m_room.GetSlot(memberID++);
            if (slot.Member.has_value() && slot.Member->Name == sender.Name)
            {
                avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
                break;
            }
        }

        if (avatar && !code.empty())
            ShowEmoticon(avatar, code);
    }

    void StateWaiting7K::OnMusicChanged(const WaitingMusicChangedEventData& ev)
    {
        InvalidateRoomInfo();
        InvalidateAvatarInfo();
    }

    void StateWaiting7K::OnTitleChanged(const WaitingTitleChangedEventData& ev)
    {
        m_room.SetTitle(ev.Title);
        InvalidateRoomInfo();
    }

    void StateWaiting7K::OnMapChanged(const WaitingMapChangedEventData& ev)
    {
        InvalidateRoomInfo();
    }

    void StateWaiting7K::OnKicked(const WaitingKickEventData& ev)
    {
        GetDirector().Dismiss<StateRoom>(RoomTransitionEventType::Kick);
    }

    void StateWaiting7K::OnStartGame(const StartGameEventData& ev)
    {
        const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START);
        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);

        if (ev.ResultCode != StartGameResult::Success)
        {
            auto message = std::string();
            switch (ev.ResultCode)
            {
                case StartGameResult::NotReady:
                {
                    message = "There are users not ready yet";
                    break;
                }
                case StartGameResult::InsufficientPlayers:
                {
                    message = "For VS mode, more than one player is required.";
                    break;
                }
                case StartGameResult::TeamUnbalanced:
                {
                    message = "The team is not in harmony.";
                    break;
                }
                default: break;
            }

            startButton->SetCheckedState(false);
            startButton->SetEnabled(true);
            backButton->SetEnabled(true);

            ShowDialog(message, DialogStyle::Information);
            return;
        }

        if (!m_game.GetChart() || m_game.GetChart()->Source != m_room.GetMusic().Source)
        {
            auto chart    = std::make_unique<Chart>();
            chart->Source = m_room.GetMusic().Source;

            m_game.SetChart(std::move(chart));
        }

        m_game.SetMode(m_room.GetMode());
        m_game.SetDifficulty(m_room.GetDifficulty());
        m_game.SetSpeed(m_room.GetSpeed());
        m_game.SetMapID(m_room.GetMapID() == MapInfo::RandomID || m_room.GetMapID() == 0 ? m_room.GetRandomizedMapID() : m_room.GetMapID());
        m_game.SetEffectID(m_room.GetEffectID());

        GetDirector().Present<StateLoading>();
    }

    void StateWaiting7K::InitializeAvatars()
    {
        const auto avatarList = Instantiate<Gx::List>(Resource::Waiting7K::IDC_LIST_AVATAR);

        int memberID = 0;
        for (const auto child : avatarList->GetChildren())
        {
            if (memberID >= RoomContext::MaxCapacity)
                break;

            const auto container = dynamic_cast<Gx::UiContainer*>(child);
            if (!container)
                continue;

            const auto avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
            if (!avatar)
                continue;

            auto& emoticonContainer = Create<Gx::UiContainer>();
            emoticonContainer.SetName(Resource::Waiting7K::Avatar::IDC_CONTAINER_EMOTICON);
            emoticonContainer.SetVisible(false);
            avatar->AddChild(emoticonContainer);

            const auto avatarInfo = avatar->FindChild<AvatarInfo>(Resource::Waiting7K::Avatar::IDC_AVATAR_INFO);
            auto& slot = m_room.GetSlot(memberID);

            if (slot.Member.has_value() && slot.Member->Name == m_session.GetCharacterInfo().Name)
            {
                m_slot       = &slot;
                m_avatarInfo = avatarInfo;
                m_mainAvatar = avatar;
            }

            memberID++;
        }
    }

    void StateWaiting7K::OnReadyStateChanged(Gx::ToggleButton& sender)
    {
        const auto teamButtons = Instantiate<Gx::UiContainer>(Resource::Waiting7K::IDC_CONTAINER_TEAM_BUTTONS);
        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);

        sender.SetEnabled(false);
        backButton->SetEnabled(false);
        teamButtons->SetEnabled(!sender.IsChecked());

        m_service.UpdateReadyState([=, &sender]
        {
            Invoke([=, &sender]
            {
                sender.SetEnabled(true);
                backButton->SetEnabled(true);
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
        });
    }

    void StateWaiting7K::OnStartStateChanged(Gx::ToggleButton& sender)
    {
         if (!sender.IsChecked())
            return;

        const auto sfxStart = Instantiate<sf::Sound>(Sound::Effects::EF_33);
        m_mixer.Play(*sfxStart, Sound::Channel::SFX);

        for (std::size_t i = 0; i < RoomContext::MaxCapacity; i++)
        {
            auto& slot = m_room.GetSlot(i);
            if (slot.State != RoomSlotState::Occupied || slot.IsMaster)
                continue;

            if (slot.Member->MusicIDs.find(m_room.GetMusic().ID) == slot.Member->MusicIDs.end())
            {
                ShowDialog("There are users who have not the right tune.", DialogStyle::Information);
                sender.SetCheckedState(false);

                return;
            }
        }

        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);
        backButton->SetEnabled(false);

        sender.SetEnabled(false);
        m_service.StartGame([=]
        {
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

    void StateWaiting7K::OnSelectMusicButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        const auto sfxSelectMusic    = Instantiate<sf::Sound>(Sound::Effects::EF_35);

        m_mixer.Play(*sfxSelectMusic, Sound::Channel::SFX);
        Present(*selectMusicDialog, Gx::PresentationContext::Default);
    }

    void StateWaiting7K::OnSelectMusicDialogAccepted()
    {
        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        const auto selectMusicButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_SELECT_MUSIC);

        selectMusicButton->SetEnabled(false);

        auto music = selectMusicDialog->GetSelectedMusic();
        m_room.SetDifficulty(selectMusicDialog->GetSelectedDifficulty());
        m_room.SetSpeed(selectMusicDialog->GetSelectedSpeed());

        m_room.SetRandomLevel(selectMusicDialog->GetSelectedRandomLevels());

        auto request = UpdateRoomMusicRequest
        {
            0,
            m_room.GetDifficulty(),
            m_room.GetSpeedID()
        };

        if (static_cast<int>(m_room.GetRandomLevel()) != 0)
        {
            const auto& musicList = m_session.GetInstalledMusic();
            const auto availableMusicIDs = m_room.GetAvailableMusicIDs();

            const auto [lvStart, lvEnd] = GetRandomLevelRange(m_room.GetRandomLevel());

            auto pool = std::vector<std::pair<Difficulty, ChartMetadata>>();
            for (const auto& header : musicList)
            {
                if (availableMusicIDs.find(header.ID) != availableMusicIDs.end())
                {
                    for (Difficulty diff : { Difficulty::EX, Difficulty::NX, Difficulty::HX })
                    {
                        if (lvStart != 0 && header.Levels.find(diff)->second < lvStart)
                            continue;

                        if (lvEnd != 0 && header.Levels.find(diff)->second > lvEnd)
                            continue;

                        pool.push_back(std::pair{diff, header});
                    }
                }
            }

            const auto [diff, music] = pool[Gx::Randomizer::Randomize<std::size_t>(0, pool.size() - 1)];

            request.MusicID    = music.ID | static_cast<int>(m_room.GetRandomLevel()) << 28;
            request.Difficulty = diff;

            m_room.SetMusic(music);
            m_room.SetDifficulty(diff);
        }
        else
        {
            m_room.SetMusic(selectMusicDialog->GetSelectedMusic());
            request.MusicID = m_room.GetMusic().ID;
        }

        m_service.UpdateMusic
        (
            request,
            [=]
            {
                Invoke([=]
                {
                    selectMusicButton->SetEnabled(true);
                    InvalidateRoomInfo();
                });
            }, [=] (const auto& ex)
            {
                Invoke([=]
                {
                    selectMusicButton->SetEnabled(true);
                    ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
                    {
                        GetDirector().Dismiss<StatePlanet>();
                    });
                });
            }
        );
    }

    void StateWaiting7K::OnChangeTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto dialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto titleBox = dialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);

        if (!m_room.GetCurrentSlot().IsMaster)
        {
            ShowDialog("Only Room master can change the room title.", DialogStyle::Information);
            return;
        }

        auto ctx   = Gx::DialogPresentationContext();
        ctx.Bounds = {{}, GetView().getSize()};
        ctx.Prompt = "Please enter a room name.";

        Present(*dialog, ctx);
        sender.SetFocus(false);

        titleBox->SetString(m_room.GetTitle());
        titleBox->SetFocus(true);
        titleBox->SelectAll();
    }

    void StateWaiting7K::OnChangeTitleDialogAccepted()
    {
        const auto dialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto titleBox = dialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);

        titleBox->SetMaximumTextLength(21);
        if (titleBox->GetString().isEmpty())
            return;

        m_service.UpdateRoomTitle(titleBox->GetString(), [=]
        {
            Invoke([=]
            {
                m_room.SetTitle(titleBox->GetString());
                InvalidateRoomInfo();
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
        });
    }

    void StateWaiting7K::OnTeamButtonStateChanged(const Gx::ToggleButton& sender, const RoomTeam team)
    {
        if (!sender.IsChecked() || !m_avatarInfo)
            return;

        m_service.UpdateTeam(team, [=]
        {
            Invoke([=]
            {
                const auto sfxTeam = Instantiate<sf::Sound>(Sound::Effects::EF_34);
                if (const auto member = m_avatarInfo->GetSlot(); member)
                    member->Team = team;

                m_avatarInfo->Invalidate();
                m_mixer.Play(*sfxTeam, Sound::Channel::SFX);
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
        });
    }

    void StateWaiting7K::OnInstrumentSelectorStateChanged(const ItemMetadata& metadata) const
    {
        auto item = m_items.Create(metadata.ID);
        if (!m_mainAvatar)
            return;

        if (item.GetID() == 0 || m_mainAvatar->IsEquiped(item))
        {
            m_mainAvatar->Unequip(EquipmentType::Bass);
            m_mainAvatar->Unequip(EquipmentType::Guitar);
            m_mainAvatar->Unequip(EquipmentType::Keyboard);
            m_mainAvatar->Unequip(EquipmentType::Drum);
        }
        else
            m_mainAvatar->Equip(std::move(item));

        auto& member = m_room.GetCurrentSlot().Member.value();
        const auto defaultItems = m_items.GetDefaultItems(member.Gender);
        member.EquippedItemIDs.clear();

        for (auto [_, equipedItem] : m_mainAvatar->GetEquipedItems())
            member.EquippedItemIDs.insert(equipedItem->GetID());
    }

    void StateWaiting7K::OnMapSelectorStateChanged(const unsigned int mapID)
    {
        if (!m_room.GetCurrentSlot().IsMaster)
            return;

        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);
        const auto mapInfo = MapInfo
        {
            static_cast<std::uint8_t>((mapID == MapInfo::RandomID || mapID == 0) ? Gx::Randomizer::Randomize<int>(1, mapSelector->GetMapCount()) : mapID),
            static_cast<std::uint16_t>(0),
            static_cast<std::uint8_t>((mapID == MapInfo::RandomID || mapID == 0) ? MapInfo::RandomID : 0)
        };

        m_room.SetMapID(mapID);
        m_room.SetRandomizedMapID(mapInfo.GetRandomizedMap());
        m_service.UpdateMap(UpdateMapRequest{ mapInfo }, [] {});
    }

    void StateWaiting7K::OnEmoticonButtonClicked(Gx::Control&, Gx::Control::Event&)
    {
        const auto emoticonDialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_EMOTICON);
        const auto content        = emoticonDialog->FindChild<Gx::Image>(Resource::Waiting7K::Emoticon::IDC_IMAGE_CONTENT);
        const auto currentPage    = emoticonDialog->FindChild<Gx::BitmapNumber>(Resource::Waiting7K::Emoticon::IDC_NUMBER_CURRENT_PAGE);
        const auto maxPage        = emoticonDialog->FindChild<Gx::BitmapNumber>(Resource::Waiting7K::Emoticon::IDC_NUMBER_MAX_PAGE);

        content->SetFrame(0);
        currentPage->SetValue(1);
        maxPage->SetValue(content->GetFrameCount());

        Present(*emoticonDialog, Gx::PresentationContext::Default);
    }

    void StateWaiting7K::OnEmoticonNextPageButtonClicked(Gx::Control&, Gx::Control::Event&)
    {
        const auto emoticonDialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_EMOTICON);
        const auto content        = emoticonDialog->FindChild<Gx::Image>(Resource::Waiting7K::Emoticon::IDC_IMAGE_CONTENT);
        const auto currentPage    = emoticonDialog->FindChild<Gx::BitmapNumber>(Resource::Waiting7K::Emoticon::IDC_NUMBER_CURRENT_PAGE);

        if (content->GetCurrentFrameIndex() < content->GetFrameCount() - 1)
        {
            content->SetFrame(content->GetCurrentFrameIndex() + 1);
            currentPage->SetValue(currentPage->GetValue() + 1);
        }
    }

    void StateWaiting7K::OnEmoticonPreviousPageButtonClicked(Gx::Control&, Gx::Control::Event&)
    {
        const auto emoticonDialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_EMOTICON);
        const auto content        = emoticonDialog->FindChild<Gx::Image>(Resource::Waiting7K::Emoticon::IDC_IMAGE_CONTENT);
        const auto currentPage    = emoticonDialog->FindChild<Gx::BitmapNumber>(Resource::Waiting7K::Emoticon::IDC_NUMBER_CURRENT_PAGE);

        if (content->GetCurrentFrameIndex() > 0)
        {
            content->SetFrame(content->GetCurrentFrameIndex() - 1);
            currentPage->SetValue(currentPage->GetValue() - 1);
        }
    }

    void StateWaiting7K::OnBackButtonClicked(Gx::Control&, Gx::Control::Event&)
    {
        m_service.ExitRoom([&]
        {
            if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_36))
                m_mixer.Play(*sfx, Sound::Channel::SFX);

            GetDirector().Dismiss();
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

    void StateWaiting7K::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        State::OnKeyPressed(ev);

        if (ev.code == sf::Keyboard::Key::F3)
        {
            if (m_room.GetCurrentSlot().IsMaster)
            {
                if (const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START))
                    startButton->SetCheckedState(true);
            }
            else
            {
                if (const auto readyButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_READY))
                    readyButton->SetCheckedState(!readyButton->IsChecked());
            }
        }


        const auto chatPanel = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        if (ev.control)
        {
            if (ev.shift)
            {
                switch (ev.code)
                {
                    case sf::Keyboard::Key::Num1:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 190, 200, 179, 231 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HI);
                        break;
                    }
                    case sf::Keyboard::Key::Num2:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 176, 237 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_GO);
                        break;
                    }
                    case sf::Keyboard::Key::Num3:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 183, 185, 181, 240 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_READY);
                        break;
                    }
                    case sf::Keyboard::Key::Num4:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 193, 193, 190, 198 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_APPROVAL);
                        break;
                    }
                    case sf::Keyboard::Key::Num5:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 189, 200, 190, 238 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OBJECTION);
                        break;
                    };
                    case sf::Keyboard::Key::Num6:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 200, 229, 200, 229 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_SMILE);
                        break;
                    };
                    case sf::Keyboard::Key::Num7:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 199, 207, 199, 207 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_HAHA);
                        break;
                    };
                    case sf::Keyboard::Key::Num8:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 197, 169, 197, 169 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_VICTORY);
                        break;
                    };
                    case sf::Keyboard::Key::Num9:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 192, 185 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_LOSE);
                        break;
                    };
                    case sf::Keyboard::Key::Num0:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 46, 46, 46 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BY_DEGREES);
                        break;
                    };
                    default:
                        break;
                }
            }
            else
            {
                switch (ev.code)
                {
                    case sf::Keyboard::Key::Num1:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 33 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK);
                        break;
                    }
                    case sf::Keyboard::Key::Num2:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 63 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_QUESTION);
                        break;
                    }
                    case sf::Keyboard::Key::Num3:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 187, 231, 182, 251, 199, 216 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HEART);
                        break;
                    }
                    case sf::Keyboard::Key::Num4:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 51, 50, 49 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_COUNT);
                        break;
                    }
                    case sf::Keyboard::Key::Num5:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 126 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_NOTE);
                        break;
                    }
                    case sf::Keyboard::Key::Num6:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 33, 33 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK2);
                        break;
                    }
                    case sf::Keyboard::Key::Num7:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 185, 204, 191, 246 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BROKEN_HEART);
                        break;
                    }
                    case sf::Keyboard::Key::Num8:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 55 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_CLOVER);
                        break;
                    }
                    case sf::Keyboard::Key::Num9:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 178, 201 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_FLOWER);
                        break;
                    }
                    case sf::Keyboard::Key::Num0:
                    {
                        auto code = std::vector<std::uint8_t>{ 47, 185, 221, 194, 166 };
                        m_messaging.SendMessage(std::string(code.begin(), code.end()), nullptr);

                        ShowEmoticon(m_mainAvatar, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_IDEA);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    void StateWaiting7K::OnKeyReleased(const sf::Event::KeyReleased& ev)
    {
        State::OnKeyReleased(ev);
    }

    void StateWaiting7K::ExtendSlot(const unsigned int slotID)
    {
        const auto& slot = m_room.GetSlot(slotID);
        if (slot.Member.has_value() && !slot.Member->Name.isEmpty())
        {
            if (slot.Member->Name == m_room.GetCurrentSlot().Member->Name)
                return;

            ShowDialog("Would you like to kick out?", DialogStyle::YesNo, false, [=] (const bool confirm)
            {
                if (confirm)
                    m_service.UpdateRoomSlot(static_cast<std::uint8_t>(slotID), nullptr);
            });
        }
        else
            m_service.UpdateRoomSlot(static_cast<std::uint8_t>(slotID), nullptr);
    }

    void StateWaiting7K::ShowEmoticon(const Avatar* avatar, const std::string& emoticonID)
    {
        const auto container = avatar->FindChild<Gx::UiContainer>(Resource::Waiting7K::Avatar::IDC_CONTAINER_EMOTICON);
        if (container->IsVisible())
            return;

        auto emoticon = container->FindChild<Gx::Animation>(emoticonID);
        if (!emoticon)
        {
            emoticon = &Create<Gx::Animation>(*Find<Gx::Animation>(emoticonID));
            emoticon->SetName(emoticonID);
            emoticon->SetAnimationCallback([=] (auto& sender)
            {
                container->SetVisible(
                    sender.GetState() == Gx::Animation::AnimationState::Initial ||
                    sender.GetState() == Gx::Animation::AnimationState::Playing
                );
                sender.SetVisible(container->IsVisible());
            });
            container->AddChild(*emoticon);
        }

        emoticon->Reset();
    }

    void StateWaiting7K::InvalidateRoomInfo()
    {
        const bool isMaster = m_room.GetCurrentSlot().IsMaster;

        const auto channelNumber = Instantiate<Gx::BitmapNumber>(Resource::Waiting7K::IDC_NUMBER_CHANNEL_ID);
        channelNumber->SetValue(m_session.GetChannelID() + 1);

        const auto roomNumber = Instantiate<Gx::BitmapNumber>(Resource::Waiting7K::IDC_NUMBER_ROOM_ID);
        roomNumber->SetValue(m_room.GetID());

        const auto m_roomName = Instantiate<Gx::Label>(Resource::Waiting7K::IDC_TEXT_ROOM_NAME);
        m_roomName->SetString(m_room.GetTitle());

        const auto musicName = Instantiate<Gx::Label>(Resource::Waiting7K::IDC_TEXT_MUSIC_NAME);
        if (static_cast<int>(m_room.GetRandomLevel()) != 0)
        {
            const auto [start, end] = GetRandomLevelRange(m_room.GetRandomLevel());

            musicName->SetColor(sf::Color(2, 222, 225));
            musicName->SetString(fmt::format(
                "<< Random {} {} >>",
                start == 0 ? 13 : start,
                end == 0 ? "over" : fmt::format("- {}", end)
            ));
        }
        else
        {
            musicName->SetColor(sf::Color::White);
            musicName->SetString(sf::String(fmt::format(L"{} [BPM: {:.2f}]", m_room.GetMusic().Title, m_room.GetMusic().BPM)));
        }

        const auto level = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_ROOM_LEVEL);
        level->SetFrame(GetRoomLevelCode(m_room));

        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);
        mapSelector->SetMapID(m_room.GetMapID(), true);
        mapSelector->SetControlsEnabled(isMaster);

        if (const auto cover = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_COVER_MUSIC))
            cover->SetVisible(!isMaster);

        if (const auto selectMusicButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_SELECT_MUSIC); selectMusicButton)
            selectMusicButton->SetEnabled(isMaster);

        const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START);
        startButton->SetVisible(isMaster);
        startButton->SetEnabled(startButton->IsVisible());

        const auto readyButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_READY);
        readyButton->SetVisible(!isMaster);
        readyButton->SetEnabled(readyButton->IsVisible());
    }

    void StateWaiting7K::InvalidateAvatarInfo()
    {
        const auto avatarList = Instantiate<Gx::List>(Resource::Waiting7K::IDC_LIST_AVATAR);
        for (const auto child : avatarList->GetChildren())
        {
            const auto container = dynamic_cast<Gx::UiContainer*>(child);
            if (!container)
                continue;

            const auto avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
            if (!avatar)
                continue;

            const auto avatarInfo = avatar->FindChild<AvatarInfo>(Resource::Waiting7K::Avatar::IDC_AVATAR_INFO);
            if (!avatarInfo || !avatarInfo->GetSlot())
                continue;

            auto& slot = *avatarInfo->GetSlot();
            if (slot.State != RoomSlotState::Occupied)
                continue;

            const auto bossMark = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_BOSS_MARK);
            const auto noMusic  = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_NO_MUSIC);

            bossMark->SetVisible(false);
            noMusic->SetVisible(false);

            bossMark->SetVisible(slot.IsMaster);
            const std::uint32_t musicID = m_room.GetMusic().ID;

            if (m_session.GetCharacterInfo().Name == slot.Member->Name)
            {
                const auto& musicList = m_session.GetInstalledMusic();

                // TODO: Get music list header without relying OJNList
                const auto it = std::find_if(musicList.begin(), musicList.end(), [musicID] (const auto& header) {
                    return musicID == header.ID;
                });

                noMusic->SetVisible(it == musicList.end());
            }
            else
                noMusic->SetVisible(slot.Member->MusicIDs.find(musicID) == slot.Member->MusicIDs.end());

            avatarInfo->Invalidate();
        }
    }

    void StateWaiting7K::InvalidateMembers()
    {
        const auto avatarList = Instantiate<Gx::List>(Resource::Waiting7K::IDC_LIST_AVATAR);

        int memberIndex = 0;
        for (const auto child : avatarList->GetChildren())
        {
            if (memberIndex >= RoomContext::MaxCapacity)
                break;

            const auto container = dynamic_cast<Gx::UiContainer*>(child);
            if (!container)
                continue;

            const auto avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
            if (!avatar)
                continue;

            const auto avatarInfo = avatar->FindChild<AvatarInfo>(Resource::Waiting7K::Avatar::IDC_AVATAR_INFO);
            avatar->ClearEquipments();
            avatarInfo->Reset();

            auto& slot = m_room.GetSlot(memberIndex);
            if (const auto cover = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_COVER_AVATAR))
                cover->SetVisible(slot.State == RoomSlotState::Locked);

            if (const auto btnExtend = container->FindChild<Gx::Button>(Resource::Waiting7K::Avatar::IDC_BUTTON_EXTEND))
            {
                btnExtend->SetEnabled(m_room.GetCurrentSlot().IsMaster);
                btnExtend->SetDoubleClickCallback([=] (const auto&, auto&) { ExtendSlot(memberIndex); });
            }

            if (slot.State == RoomSlotState::Unoccupied || (slot.State == RoomSlotState::Occupied && !slot.Member.has_value()))
            {
                avatar->SetVisible(false);
                memberIndex++;
                continue;
            }

            avatar->SetVisible(true);
            avatar->SetGender(Gender::Any);

            const auto bossMark = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_BOSS_MARK);
            const auto noMusic  = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_NO_MUSIC);

            bossMark->SetVisible(false);
            noMusic->SetVisible(false);

            if (slot.State == RoomSlotState::Locked)
            {
                memberIndex++;
                continue;
            }

            bossMark->SetVisible(slot.IsMaster);
            const std::uint32_t musicID = m_room.GetMusic().ID;

            if (m_session.GetCharacterInfo().Name == slot.Member->Name)
            {
                const auto& musicList = m_session.GetInstalledMusic();

                // TODO: Get music list header without relying OJNList
                const auto it = std::find_if(musicList.begin(), musicList.end(), [musicID] (const auto& header) {
                    return musicID == header.ID;
                });

                noMusic->SetVisible(it == musicList.end());
            }
            else
                noMusic->SetVisible(slot.Member->MusicIDs.find(musicID) == slot.Member->MusicIDs.end());

            avatar->SetGender(slot.Member->Gender);
            avatarInfo->SetSlot(slot);

            for (auto [_, item]: m_items.GetDefaultItems(slot.Member->Gender))
                avatar->SetDefaultItem(std::move(item));

            for (const auto itemID : slot.Member->EquippedItemIDs)
                avatar->Equip(m_items.Create(itemID));

            memberIndex++;
        }
    }

}
