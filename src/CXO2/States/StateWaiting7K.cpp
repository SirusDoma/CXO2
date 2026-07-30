#include <CXO2/States/StateWaiting7K.hpp>

#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StatePlaying7K.hpp>

#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Models/Map.hpp>
#include <CXO2/Services/WaitingService.hpp>
#include <CXO2/Services/ChatService.hpp>

#include <CXO2/Network/Requests/UpdateMapRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Network/Events/StartGameEventData.hpp>
#include <CXO2/Network/Events/WaitingMemberJoinedEventData.hpp>
#include <CXO2/Network/Events/WaitingMemberLeftEventData.hpp>
#include <CXO2/Network/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <CXO2/Network/Events/WaitingMemberTeamChangedEventData.hpp>
#include <CXO2/Network/Events/WaitingSlotChangedEventData.hpp>
#include <CXO2/Network/Events/WaitingMapChangedEventData.hpp>
#include <CXO2/Network/Events/WaitingMusicChangedEventData.hpp>
#include <CXO2/Network/Events/WaitingTitleChangedEventData.hpp>

#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/UI/Waiting/MapSelector.hpp>
#include <CXO2/UI/Waiting/InstrumentSelector.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Waiting7K.hpp>
#include <CXO2/Constants/Messages/Chat.hpp>
#include <CXO2/Constants/Messages/Room.hpp>
#include <CXO2/Constants/Messages/Waiting.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <limits>
#include <unordered_map>

namespace Cx
{
    using namespace Constants::Identifiers;

    namespace
    {
        const std::unordered_map<std::string, std::string> EmoticonMap = {
            { Constants::Messages::Chat::Emoticons::EXCLAMATION_MARK,  Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK },
            { Constants::Messages::Chat::Emoticons::QUESTION,          Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_QUESTION },
            { Constants::Messages::Chat::Emoticons::HEART,             Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HEART },
            { Constants::Messages::Chat::Emoticons::COUNT,             Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_COUNT },
            { Constants::Messages::Chat::Emoticons::NOTE,              Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_NOTE },
            { Constants::Messages::Chat::Emoticons::EXCLAMATION_MARK2, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK2 },
            { Constants::Messages::Chat::Emoticons::BROKEN_HEART,      Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BROKEN_HEART },
            { Constants::Messages::Chat::Emoticons::CLOVER,            Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_CLOVER },
            { Constants::Messages::Chat::Emoticons::FLOWER,            Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_FLOWER },
            { Constants::Messages::Chat::Emoticons::IDEA,              Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_IDEA },
            { Constants::Messages::Chat::Emoticons::HI,                Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HI },
            { Constants::Messages::Chat::Emoticons::GO,                Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_GO },
            { Constants::Messages::Chat::Emoticons::READY,             Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_READY },
            { Constants::Messages::Chat::Emoticons::APPROVAL,          Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_APPROVAL },
            { Constants::Messages::Chat::Emoticons::OBJECTION,         Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OBJECTION },
            { Constants::Messages::Chat::Emoticons::OHM_SMILE,         Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_SMILE },
            { Constants::Messages::Chat::Emoticons::OHM_HAHA,          Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_HAHA },
            { Constants::Messages::Chat::Emoticons::OHM_VICTORY,       Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_VICTORY },
            { Constants::Messages::Chat::Emoticons::OHM_LOSE,          Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_LOSE },
            { Constants::Messages::Chat::Emoticons::BY_DEGREES,        Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BY_DEGREES },
        };

        std::string GetRoomLevelCode(const RoomContext& room, const bool useNormalMode = false)
        {
            const auto mode = room.GetSpeedMode();

            std::string speedStr = "R";
            if (mode != SpeedMode::XrSpeed && mode != SpeedMode::TdSpeed && room.GetSpeed() > 0)
                speedStr = fmt::format("{}", room.GetSpeed());

            if (useNormalMode || !room.IsRandomActive())
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

            if (room.IsRandomActive())
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

    StateWaiting7K::StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, WaitingService& service, ChatService& messaging, ItemFactory& items) :
        m_mixer(mixer),
        m_session(session),
        m_room(room),
        m_service(service),
        m_messaging(messaging),
        m_items(items)
    {
    }

    void StateWaiting7K::Initialize()
    {
        State::Initialize();

        RegisterMessageEvents();

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
        auto teamButtonMatcher = [=] (const Room::Team team) -> Gx::RadioButton*
        {
            switch (team)
            {
                default:
                case Room::Team::A: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_A);
                case Room::Team::B: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_B);
                case Room::Team::C: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_C);
                case Room::Team::D: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_D);
                case Room::Team::E: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_E);
                case Room::Team::F: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_F);
                case Room::Team::G: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_G);
                case Room::Team::H: return teamButtons->FindChild<Gx::RadioButton>(Resource::Waiting7K::Team::IDC_RADIO_TEAM_H);
            }
        };

        if (m_slot)
        {
            if (const auto currentTeamButton = teamButtonMatcher(m_slot->Team); currentTeamButton)
                currentTeamButton->SetCheckedState(true);
        }

        for (const auto team : { Room::Team::A, Room::Team::B, Room::Team::C, Room::Team::D, Room::Team::E, Room::Team::F, Room::Team::G, Room::Team::H })
        {
            const auto teamButton = teamButtonMatcher(team);
            if (!teamButton)
                continue;

            m_teamButtons[teamButton] = team;
            teamButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnTeamButtonStateChanged(sender, ev); });
        }

        // Emoticon dialog
        const auto emoticonDialog      = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_EMOTICON);
        const auto emoticonPrevButton  = emoticonDialog->FindChild<Gx::Button>(Resource::Waiting7K::Emoticon::IDC_BUTTON_LEFT);
        const auto emoticonNextButton  = emoticonDialog->FindChild<Gx::Button>(Resource::Waiting7K::Emoticon::IDC_BUTTON_RIGHT);

        emoticonPrevButton->SetClickCallback([this] (auto& sender, auto& ev) { OnEmoticonPreviousPageButtonClicked(sender, ev); });
        emoticonNextButton->SetClickCallback([this] (auto& sender, auto& ev) { OnEmoticonNextPageButtonClicked(sender, ev); });

        const auto emoticonHelpButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_EMOTICON);
        emoticonHelpButton->SetClickCallback([this] (auto& sender, auto& ev) { OnEmoticonButtonClicked(sender, ev); });

        // Change title dialog
        const auto changeTitleDialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto changeTitleBox = changeTitleDialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);
        changeTitleBox->SetMaximumTextLength(21);
        changeTitleDialog->SetAcceptCallback([this] { OnChangeTitleDialogAccepted(); });

        const auto changeTitleButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_CHANGE_TITLE);
        changeTitleButton->SetClickCallback([this] (auto& sender, auto& ev) { OnChangeTitleButtonClicked(sender, ev); });

        // Map selector
        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);
        mapSelector->SetMapChangedCallback([this] (const unsigned int mapID) { OnMapSelectorStateChanged(mapID); });
        mapSelector->SetEffectChangedCallback([this] (const unsigned int effectID) { OnEffectSelectorStateChanged(effectID); });

        mapSelector->SetMapID(m_room.GetMap().Random ? 0 : m_room.GetMap().ID, true);
        mapSelector->SetEffectID(m_room.GetEffectID());
        mapSelector->SetControlsEnabled(m_room.GetCurrentSlot().IsMaster);

        // Instrument selector
        const auto instrumentSelector = Instantiate<InstrumentSelector>(Resource::Waiting7K::IDC_CONTAINER_INSTRUMENT_SELECTOR);
        instrumentSelector->SetInstrumentSelectCallback([this] (const auto& metadata) { OnInstrumentSelectorStateChanged(metadata); });

        for (const auto id : m_session.GetInventory())
            instrumentSelector->AddInstrumentMetadata(m_items.GetItemMetadata(id));

        for (const auto id : m_session.GetEquippedItemIDs())
            instrumentSelector->AddInstrumentMetadata(m_items.GetItemMetadata(id));

        // Select music dialog
        const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
        selectMusicDialog->SetAcceptCallback([this] { OnSelectMusicDialogAccepted(); });

        const auto selectMusicButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_SELECT_MUSIC);
        selectMusicButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSelectMusicButtonClicked(sender, ev); });

        // Chat panel & window
        const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        chatPanel->SetMaximumTextLength(50);

        const auto chatWindow = chatPanel->GetChatWindow();
        chatWindow->PushSystemMessage("Welcome to O2Jam!");
        chatWindow->PushSystemMessage("Let's play together~");
        chatWindow->PushSystemMessage(Constants::Messages::Waiting::Welcome::CHANGE_TITLE);

        if (const auto cover = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_COVER_MUSIC))
            cover->SetVisible(!m_room.GetCurrentSlot().IsMaster);

        // Buttons
        const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START);
        startButton->SetVisible(m_room.GetCurrentSlot().IsMaster);
        startButton->SetEnabled(startButton->IsVisible());
        startButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnStartStateChanged(sender, ev); });

        const auto readyButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_READY);
        readyButton->SetVisible(!m_room.GetCurrentSlot().IsMaster);
        readyButton->SetEnabled(readyButton->IsVisible());
        readyButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnReadyStateChanged(sender, ev); });

        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);
        backButton->SetClickCallback([this] (auto& sender, auto& ev) { OnBackButtonClicked(sender, ev); });

        InvalidateRoomInfo();
        InvalidateMembers();

        if (m_room.GetCurrentSlot().IsMaster && m_room.IsRandomActive())
        {
            selectMusicDialog->Initialize();
            OnSelectMusicDialogAccepted();
        }

        if (m_room.GetCurrentSlot().IsMaster && (m_room.GetMap().Random || m_room.GetMap().ID == 0))
            OnMapSelectorStateChanged(0);

        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
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
            const auto& slot = m_room.GetSlot(memberID);

            if (slot.State == Room::SlotState::Occupied && slot.Name == m_session.GetName())
            {
                m_slot       = &slot;
                m_avatarInfo = avatarInfo;
                m_mainAvatar = avatar;
            }

            memberID++;
        }
    }

    void StateWaiting7K::RegisterMessageEvents()
    {
        m_service.SetSlotChangedEventCallback([this] (const auto& ev) { OnSlotChanged(ev); });
        m_service.SetMemberJoinedEventCallback([this] (const auto& ev) { OnMemberJoined(ev); });
        m_service.SetMemberLeftEventCallback([this] (const auto& ev) { OnMemberLeft(ev); });
        m_service.SetMemberTeamChangedEventCallback([this] (const auto& ev) { OnMemberTeamChanged(ev); });
        m_service.SetMemberReadyStateChangedEventCallback([this] (const auto& ev) { OnMemberReadyStateChanged(ev); });
        m_service.SetMusicChangedEventCallback([this] (const auto& ev) { OnMusicChanged(ev); });
        m_service.SetTitleChangedEventCallback([this] (const auto& ev) { OnTitleChanged(ev); });
        m_service.SetMapChangedEventCallback([this] (const auto& ev) { OnMapChanged(ev); });
        m_service.SetKickedEventCallback([this] (const auto& ev) { OnKicked(ev); });
        m_service.SetStartGameEventCallback([this] (const auto& ev) { OnStartGame(ev); });
    }

    void StateWaiting7K::OnSlotChanged(const MessageEnvelope<WaitingSlotChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            if (response.ID < 0 || response.ID >= RoomContext::MaxCapacity)
                return;

            switch (response.Type)
            {
                case Room::SlotEventType::Unlock:
                {
                    assert(m_room.GetSlot(response.ID).State != Room::SlotState::Occupied && "Slot is not empty");

                    m_room.Unlock(response.ID);
                    InvalidateMembers();
                    break;
                }
                case Room::SlotEventType::Lock:
                {
                    assert(m_room.GetSlot(response.ID).State != Room::SlotState::Occupied && "Slot is not empty");

                    m_room.Lock(response.ID);
                    InvalidateMembers();
                    break;
                }
                case Room::SlotEventType::Kicked:
                {
                    assert(m_room.GetSlot(response.ID).State == Room::SlotState::Occupied && "Slot is empty");

                    auto name = m_room.GetSlot(response.ID).Name;
                    m_room.Vacate(response.ID);

                    const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
                    const auto chatWindow = chatPanel->GetChatWindow();
                    chatWindow->PushSystemMessage(fmt::format(Constants::Messages::Waiting::Members::FORCED_OUT, name));

                    InvalidateMembers();
                    break;
                }
            }
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnKicked(const MessageEnvelope<WaitingKickEventData>& ev)
    {
        try
        {
            const auto& _ = ev.Open();

            m_room.Leave();
            GetDirector().Dismiss<StateRoom>(RoomTransitionEventType::Kick);
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnMemberJoined(const MessageEnvelope<WaitingMemberJoinedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            if (response.ID < 0 || response.ID >= RoomContext::MaxCapacity)
                return;

            auto member = RoomContext::Member{};
            member.Name            = response.Name;
            member.Gender          = response.Gender;
            member.Level           = response.Level;
            member.EquippedItemIDs = response.EquippedItemIDs;
            member.MusicIDs        = response.MusicIDs;

            m_room.Seat(response.ID, member, response.Team, response.Ready);

            const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
            const auto chatWindow = chatPanel->GetChatWindow();

            chatWindow->PushSystemMessage(fmt::format(Constants::Messages::Waiting::Members::JOINED, m_room.GetSlot(response.ID).Name));
            InvalidateMembers();

            const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
            selectMusicDialog->Initialize();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnMemberLeft(const MessageEnvelope<WaitingMemberLeftEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            if (response.ID < 0 || response.ID >= RoomContext::MaxCapacity)
                return;

            const auto& slot = m_room.GetSlot(response.ID);
            auto name = slot.Name;

            if (slot.IsMaster)
            {
                if (response.RoomMasterID < 0 || response.RoomMasterID >= RoomContext::MaxCapacity)
                    return;

                m_room.PromoteMaster(response.RoomMasterID);
            }

            m_room.Vacate(response.ID);

            const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
            const auto chatWindow = chatPanel->GetChatWindow();
            chatWindow->PushSystemMessage(fmt::format(Constants::Messages::Waiting::Members::LEFT, name));

            InvalidateMembers();
            InvalidateRoomInfo();

            const auto selectMusicDialog = Instantiate<SelectMusicDialog>(Resource::Waiting7K::IDC_DIALOG_SELECT_MUSIC);
            selectMusicDialog->Initialize();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnMemberTeamChanged(const MessageEnvelope<WaitingMemberTeamChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            if (response.ID < 0 || response.ID >= RoomContext::MaxCapacity)
                return;

            m_room.SetTeam(response.ID, response.Team);

            InvalidateAvatarInfo();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnMemberReadyStateChanged(const MessageEnvelope<WaitingMemberReadyStateChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            if (response.ID < 0 || response.ID >= RoomContext::MaxCapacity)
                return;

            m_room.SetReady(response.ID, response.Ready);

            InvalidateAvatarInfo();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::ShowChatHelp()
    {
        const auto chatPanel  = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
        const auto chatWindow = chatPanel->GetChatWindow();

        chatWindow->PushSystemMessage(Constants::Messages::Waiting::ChatHelp::WHISPER);
        chatWindow->PushSystemMessage(Constants::Messages::Waiting::ChatHelp::ROOM_TITLE);
        chatWindow->PushSystemMessage(Constants::Messages::Waiting::ChatHelp::EFFECT_MODE);
        chatWindow->PushSystemMessage(Constants::Messages::Waiting::ChatHelp::CURSOR_MODE);
        chatWindow->PushSystemMessage(Constants::Messages::Waiting::ChatHelp::EMOTION);
    }

    void StateWaiting7K::ChangeRoomTitle(const sf::String& title)
    {
        if (!m_room.GetCurrentSlot().IsMaster)
        {
            const auto chatPanel = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
            chatPanel->GetChatWindow()->PushSystemMessage(Constants::Messages::Waiting::ChangeRoomTitle::TITLE_CHANGE_FORBIDDEN);

            return;
        }

        if (title.isEmpty())
            return;

        m_service.UpdateRoomTitle(UpdateRoomTitleRequest{title}, [=] (const auto& ev) { OnUpdateRoomTitleResponded(ev); });
    }

    void StateWaiting7K::OnMemberEmoticon(const sf::String& sender, const sf::String& chatData)
    {
        const auto prefix = std::string(Constants::Messages::Chat::Emoticons::PREFIX);
        const auto text   = chatData.toAnsiString();
        if (text.compare(0, prefix.size(), prefix) != 0)
            return;

        const auto emoticon = EmoticonMap.find(text.substr(prefix.size()));
        if (emoticon == EmoticonMap.end())
        {
            if (sender == m_session.GetName())
            {
                const auto chatPanel = Instantiate<ChatPanel>(Resource::Waiting7K::IDC_CHAT_PANEL);
                chatPanel->GetChatWindow()->PushSystemMessage(Constants::Messages::Chat::EMOTION_HINT);
            }

            return;
        }

        const auto code = emoticon->second;
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
            if (slot.State == Room::SlotState::Occupied && slot.Name == sender)
            {
                avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
                break;
            }
        }

        if (avatar && !code.empty())
            ShowEmoticon(avatar, code);
    }

    void StateWaiting7K::OnMusicChanged(const MessageEnvelope<WaitingMusicChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            const auto music = MusicID::From(response.MusicID);

            m_room.SetMusicID(music.Value);
            m_room.SetRandomLevel(music.RandomLevel);
            m_room.SetDifficulty(response.Difficulty);
            m_room.SetSpeedID(response.Speed);

            InvalidateRoomInfo();
            InvalidateAvatarInfo();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnTitleChanged(const MessageEnvelope<WaitingTitleChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            m_room.SetTitle(response.Title);
            InvalidateRoomInfo();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnMapChanged(const MessageEnvelope<WaitingMapChangedEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();
            m_room.SetMap(Map::From(response.Map));

            InvalidateRoomInfo();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnStartGame(const MessageEnvelope<StartGameEventData>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            const auto startButton = Instantiate<Gx::ToggleButton>(Resource::Waiting7K::IDC_BUTTON_START);
            const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);

            if (response.ResultCode != StartGameResult::Success)
            {
                auto message = std::string();
                switch (response.ResultCode)
                {
                    case StartGameResult::NotReady:
                    {
                        message = Constants::Messages::Waiting::GameStart::PLAYERS_NOT_READY;
                        break;
                    }
                    case StartGameResult::InsufficientPlayers:
                    {
                        message = Constants::Messages::Waiting::GameStart::INSUFFICIENT_PLAYERS;
                        break;
                    }
                    case StartGameResult::TeamUnbalanced:
                    {
                        message = Constants::Messages::Waiting::GameStart::TEAMS_UNBALANCED;
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

            GetDirector().Present<StateLoading>(m_room.CreateGameContext());
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnReadyStateChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev)
    {
        const auto teamButtons = Instantiate<Gx::UiContainer>(Resource::Waiting7K::IDC_CONTAINER_TEAM_BUTTONS);
        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);

        sender.SetEnabled(false);
        backButton->SetEnabled(false);
        teamButtons->SetEnabled(!sender.IsChecked());

        m_service.UpdateReadyState([=, &sender] (const auto& ev) { OnUpdateReadyStateResponded(sender, ev); });
    }

    void StateWaiting7K::OnUpdateReadyStateResponded(Gx::ToggleButton& sender, const MessageEnvelope<UpdateMemberReadyStateRequest>& ev)
    {
        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);

        try
        {
            const auto& _ = ev.Open();

            sender.SetEnabled(true);
            backButton->SetEnabled(true);
        }
        catch (const Gx::Exception& e)
        {
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnStartStateChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev)
    {
         if (!sender.IsChecked())
            return;

        const auto sfxStart = Instantiate<sf::Sound>(Sound::Effects::EF_33);
        m_mixer.Play(*sfxStart, Sound::Channel::SFX);

        if (m_room.GetMusic().ID == 0)
        {
            ShowDialog(Constants::Messages::Waiting::TUNE_NOT_SELECTED, DialogStyle::Information);
            sender.SetCheckedState(false);

            return;
        }

        for (const auto& meta : m_session.GetNonPlayableMusicList())
        {
            if (meta.ID == m_room.GetMusic().ID)
            {
                auto prompt = sf::String();
                if (meta.Status == MusicStatus::Unacquired)
                    prompt = Constants::Messages::Waiting::TUNE_NOT_PURCHASED;
                else
                    prompt = Constants::Messages::Waiting::TUNE_NOT_FOUND;

                ShowDialog(prompt, DialogStyle::Information);
                sender.SetCheckedState(false);

                return;
            }
        }

        for (std::size_t i = 0; i < RoomContext::MaxCapacity; i++)
        {
            const auto& slot = m_room.GetSlot(i);
            if (slot.State != Room::SlotState::Occupied || slot.IsMaster)
                continue;

            if (slot.MusicIDs.find(m_room.GetMusic().ID) == slot.MusicIDs.end())
            {
                ShowDialog(Constants::Messages::Waiting::TUNE_MISSING_FOR_OTHERS, DialogStyle::Information);
                sender.SetCheckedState(false);

                return;
            }
        }

        const auto backButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_BACK);
        backButton->SetEnabled(false);

        sender.SetEnabled(false);
        m_service.StartGame([=] (const auto& ev) { OnStartGameResponded(ev); });
    }

    void StateWaiting7K::OnStartGameResponded(const MessageEnvelope<StartGameRequest>& ev)
    {
        try
        {
            const auto& _ = ev.Open();
        }
        catch (const Gx::Exception& e)
        {
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
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
        m_room.SetSpeedMode(selectMusicDialog->GetSelectedSpeedMode());

        m_room.SetRandomLevel(selectMusicDialog->GetSelectedRandomLevels());

        auto request = UpdateRoomMusicRequest
        {
            0,
            m_room.GetDifficulty(),
            m_room.GetSpeedID()
        };

        if (m_room.IsRandomActive())
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

            request.MusicID    = MusicID::Of(music.ID, m_room.GetRandomLevel());
            request.Difficulty = diff;

            m_room.SetMusic(music);
            m_room.SetDifficulty(diff);
        }
        else
        {
            m_room.SetMusic(selectMusicDialog->GetSelectedMusic());
            request.MusicID = m_room.GetMusic().ID;
        }

        m_service.UpdateMusic(request, [=] (const auto& ev) { OnUpdateMusicResponded(ev); });
    }

    void StateWaiting7K::OnUpdateMusicResponded(const MessageEnvelope<UpdateRoomMusicRequest>& ev)
    {
        const auto selectMusicButton = Instantiate<Gx::Button>(Resource::Waiting7K::IDC_BUTTON_SELECT_MUSIC);

        try
        {
            const auto& _ = ev.Open();

            selectMusicButton->SetEnabled(true);
            InvalidateRoomInfo();
        }
        catch (const Gx::Exception& e)
        {
            selectMusicButton->SetEnabled(true);
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnChangeTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto dialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto titleBox = dialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);

        if (!m_room.GetCurrentSlot().IsMaster)
        {
            ShowDialog(Constants::Messages::Waiting::ChangeRoomTitle::TITLE_CHANGE_FORBIDDEN, DialogStyle::Information);
            return;
        }

        auto ctx   = Gx::DialogPresentationContext();
        ctx.Bounds = {{}, GetView().getSize()};
        ctx.Prompt = Constants::Messages::Waiting::ChangeRoomTitle::TITLE_PROMPT;

        Present(*dialog, ctx);
        sender.SetFocus(false);

        titleBox->SetMaximumTextLength(21);
        titleBox->SetString(m_room.GetTitle());
        titleBox->SetFocus(true);
        titleBox->SelectAll();
    }

    void StateWaiting7K::OnChangeTitleDialogAccepted()
    {
        const auto dialog = Instantiate<Gx::Dialog>(Resource::Waiting7K::IDC_DIALOG_CHANGE_TITLE);
        const auto titleBox = dialog->FindChild<Gx::InputField>(Resource::Waiting7K::ChangeTitle::IDC_EDIT_TITLE);

        if (titleBox->GetString().getSize() > titleBox->GetMaximumTextLength())
        {
            ShowDialog(Constants::Messages::Waiting::ChangeRoomTitle::TITLE_TOO_LONG, DialogStyle::Information);
        }

        if (titleBox->GetString().isEmpty())
            return;

        m_service.UpdateRoomTitle(UpdateRoomTitleRequest{titleBox->GetString()}, [=] (const auto& ev) { OnUpdateRoomTitleResponded(ev); });
    }

    void StateWaiting7K::OnUpdateRoomTitleResponded(const MessageEnvelope<UpdateRoomTitleRequest>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            m_room.SetTitle(response.Title);
            InvalidateRoomInfo();
        }
        catch (const Gx::Exception& e)
        {
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
    }

    void StateWaiting7K::OnTeamButtonStateChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        if (!sender.IsChecked() || !m_avatarInfo)
            return;

        const auto team = m_teamButtons.at(&sender);
        m_service.UpdateTeam(UpdateMemberTeamRequest{team}, [=] (const auto& ev) { OnUpdateTeamResponded(ev); });
    }

    void StateWaiting7K::OnUpdateTeamResponded(const MessageEnvelope<UpdateMemberTeamRequest>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            const auto sfxTeam = Instantiate<sf::Sound>(Sound::Effects::EF_34);
            if (const auto index = m_room.GetCurrentSlotIndex(); index < RoomContext::MaxCapacity && m_avatarInfo->GetSlot())
            {
                m_room.SetTeam(index, response.Team);
                m_room.SetTeamColor(index, m_avatarInfo->ResolveTeamColor());
            }

            m_avatarInfo->Invalidate();
            m_mixer.Play(*sfxTeam, Sound::Channel::SFX);
        }
        catch (const Gx::Exception& e)
        {
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
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

        const auto defaultItems = m_items.GetDefaultItems(m_room.GetCurrentSlot().Gender);

        auto equipment = EquipmentSet{};
        for (auto [_, equipedItem] : m_mainAvatar->GetEquipedItems())
            equipment.insert(equipedItem->GetID());

        m_room.SetEquipment(m_room.GetCurrentSlotIndex(), equipment);
    }

    void StateWaiting7K::OnMapSelectorStateChanged(const unsigned int mapID)
    {
        if (!m_room.GetCurrentSlot().IsMaster)
            return;

        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);

        const auto random = mapID == Map::RandomID || mapID == 0;
        const auto map    = Map::Of(random ? static_cast<std::uint8_t>(Gx::Randomizer::Randomize<int>(1, mapSelector->GetMapCount())) : static_cast<std::uint8_t>(mapID), random);

        m_room.SetMap(map);
        m_service.UpdateMap(UpdateMapRequest{ map });
    }

    void StateWaiting7K::OnEffectSelectorStateChanged(const unsigned int effectID)
    {
        m_room.SetEffectID(effectID);
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

    void StateWaiting7K::OnExtendButtonDoubleClicked(Gx::Control& sender, Gx::Control::Event&)
    {
        ExtendSlot(m_extendButtonSlotIDs.at(&sender));
    }

    void StateWaiting7K::OnBackButtonClicked(Gx::Control&, Gx::Control::Event&)
    {
        m_service.ExitRoom([this] (const auto& ev) { OnExitRoomResponded(ev); });
    }

    void StateWaiting7K::OnExitRoomResponded(const MessageEnvelope<ExitWaitingResponse>& ev)
    {
        try
        {
            const auto& _ = ev.Open();

            m_room.Leave();

            if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_36))
                m_mixer.Play(*sfx, Sound::Channel::SFX);

            GetDirector().Dismiss();
        }
        catch (const Gx::Exception& e)
        {
            ShowDialog(std::string(e.what()), DialogStyle::Information, false, [this] (bool)
            {
                GetDirector().Dismiss<StatePlanet>();
            });
        }
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
                        SendEmoticon(Constants::Messages::Chat::Emoticons::HI, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HI);
                        break;
                    }
                    case sf::Keyboard::Key::Num2:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::GO, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_GO);
                        break;
                    }
                    case sf::Keyboard::Key::Num3:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::READY, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_READY);
                        break;
                    }
                    case sf::Keyboard::Key::Num4:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::APPROVAL, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_APPROVAL);
                        break;
                    }
                    case sf::Keyboard::Key::Num5:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::OBJECTION, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OBJECTION);
                        break;
                    };
                    case sf::Keyboard::Key::Num6:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::OHM_SMILE, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_SMILE);
                        break;
                    };
                    case sf::Keyboard::Key::Num7:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::OHM_HAHA, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_HAHA);
                        break;
                    };
                    case sf::Keyboard::Key::Num8:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::OHM_VICTORY, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_VICTORY);
                        break;
                    };
                    case sf::Keyboard::Key::Num9:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::OHM_LOSE, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_OHM_LOSE);
                        break;
                    };
                    case sf::Keyboard::Key::Num0:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::BY_DEGREES, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BY_DEGREES);
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
                        SendEmoticon(Constants::Messages::Chat::Emoticons::EXCLAMATION_MARK, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK);
                        break;
                    }
                    case sf::Keyboard::Key::Num2:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::QUESTION, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_QUESTION);
                        break;
                    }
                    case sf::Keyboard::Key::Num3:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::HEART, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_HEART);
                        break;
                    }
                    case sf::Keyboard::Key::Num4:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::COUNT, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_COUNT);
                        break;
                    }
                    case sf::Keyboard::Key::Num5:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::NOTE, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_NOTE);
                        break;
                    }
                    case sf::Keyboard::Key::Num6:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::EXCLAMATION_MARK2, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK2);
                        break;
                    }
                    case sf::Keyboard::Key::Num7:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::BROKEN_HEART, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_BROKEN_HEART);
                        break;
                    }
                    case sf::Keyboard::Key::Num8:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::CLOVER, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_CLOVER);
                        break;
                    }
                    case sf::Keyboard::Key::Num9:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::FLOWER, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_FLOWER);
                        break;
                    }
                    case sf::Keyboard::Key::Num0:
                    {
                        SendEmoticon(Constants::Messages::Chat::Emoticons::IDEA, Resource::Waiting7K::Emoticon::IDC_ANIMATION_EMOTICON_IDEA);
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
        if (slot.State == Room::SlotState::Occupied && !slot.Name.isEmpty())
        {
            if (slot.Name == m_room.GetCurrentSlot().Name)
                return;

            ShowDialog(Constants::Messages::Waiting::Members::KICK_CONFIRM, DialogStyle::YesNo, false, [=] (const bool confirm)
            {
                if (confirm)
                    m_service.UpdateRoomSlot(UpdateRoomSlotRequest{static_cast<std::uint8_t>(slotID)});
            });
        }
        else
            m_service.UpdateRoomSlot(UpdateRoomSlotRequest{static_cast<std::uint8_t>(slotID)});
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

    void StateWaiting7K::SendEmoticon(const std::string& command, const std::string& emoticonID)
    {
        m_messaging.SendWaitingMessage(WaitingMessageRequest{Constants::Messages::Chat::Emoticons::PREFIX + command});

        ShowEmoticon(m_mainAvatar, emoticonID);
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
        if (m_room.IsRandomActive())
        {
            const auto [start, end] = GetRandomLevelRange(m_room.GetRandomLevel());

            musicName->SetColor(sf::Color(2, 222, 225));
            if (start != 0 && end != 0)
                musicName->SetString(fmt::format(Constants::Messages::Room::Random::RANGE, start, end));
            else
                musicName->SetString(fmt::format(Constants::Messages::Waiting::Music::RANDOM_OVER, start == 0 ? 13 : start));
        }
        else
        {
            musicName->SetColor(sf::Color::White);
            musicName->SetString(sf::String(fmt::format(U"{} [BPM: {:.2f}]", m_room.GetMusic().Title, m_room.GetMusic().BPM)));
        }

        const auto level = Instantiate<Gx::Image>(Resource::Waiting7K::IDC_IMAGE_ROOM_LEVEL);
        level->SetFrame(GetRoomLevelCode(m_room));

        const auto mapSelector = Instantiate<MapSelector>(Resource::Waiting7K::IDC_CONTAINER_MAP_SELECTOR);
        mapSelector->SetMapID(m_room.GetMap().Random ? 0 : m_room.GetMap().ID, true);
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

        int memberIndex = 0;
        for (const auto child : avatarList->GetChildren())
        {
            const auto container = dynamic_cast<Gx::UiContainer*>(child);
            if (!container)
                continue;

            const auto index = memberIndex++;
            const auto avatar = container->FindChild<Avatar>(Resource::Waiting7K::Avatar::IDC_AVATAR);
            if (!avatar)
                continue;

            const auto avatarInfo = avatar->FindChild<AvatarInfo>(Resource::Waiting7K::Avatar::IDC_AVATAR_INFO);
            if (!avatarInfo || !avatarInfo->GetSlot())
                continue;

            const auto& slot = *avatarInfo->GetSlot();
            if (slot.State != Room::SlotState::Occupied)
                continue;

            m_room.SetTeamColor(index, avatarInfo->ResolveTeamColor());

            const auto bossMark = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_BOSS_MARK);
            const auto noMusic  = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_NO_MUSIC);

            bossMark->SetVisible(false);
            noMusic->SetVisible(false);

            InvalidateSlotMarkers(bossMark, noMusic, slot);

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

            const auto& slot = m_room.GetSlot(memberIndex);
            if (const auto cover = avatar->FindChild<Gx::Sprite>(Resource::Waiting7K::Avatar::IDC_IMAGE_COVER_AVATAR))
                cover->SetVisible(slot.State == Room::SlotState::Locked);

            if (const auto btnExtend = container->FindChild<Gx::Button>(Resource::Waiting7K::Avatar::IDC_BUTTON_EXTEND))
            {
                btnExtend->SetEnabled(m_room.GetCurrentSlot().IsMaster);
                m_extendButtonSlotIDs[btnExtend] = memberIndex;
                btnExtend->SetDoubleClickCallback([this] (auto& sender, auto& ev) { OnExtendButtonDoubleClicked(sender, ev); });
            }

            if (slot.State == Room::SlotState::Unoccupied || (slot.State == Room::SlotState::Occupied && slot.Name.isEmpty()))
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

            if (slot.State == Room::SlotState::Locked)
            {
                memberIndex++;
                continue;
            }

            InvalidateSlotMarkers(bossMark, noMusic, slot);

            avatar->SetGender(slot.Gender);
            avatarInfo->SetSlot(slot);
            m_room.SetTeamColor(memberIndex, avatarInfo->ResolveTeamColor());

            for (auto [_, item]: m_items.GetDefaultItems(slot.Gender))
                avatar->SetDefaultItem(std::move(item));

            for (const auto itemID : slot.EquippedItemIDs)
                avatar->Equip(m_items.Create(itemID));

            memberIndex++;
        }
    }

    void StateWaiting7K::InvalidateSlotMarkers(Gx::Sprite* bossMark, Gx::Sprite* noMusic, const Room::Slot& slot)
    {
        bossMark->SetVisible(slot.IsMaster);
        const std::uint32_t musicID = m_room.GetMusic().ID;

        if (m_session.GetName() == slot.Name)
        {
            const auto& musicList = m_session.GetInstalledMusic();

            // TODO: Get music list header without relying OJNList
            const auto it = std::find_if(musicList.begin(), musicList.end(), [musicID] (const auto& header) {
                return musicID == header.ID;
            });

            noMusic->SetVisible(it == musicList.end());
        }
        else
            noMusic->SetVisible(slot.MusicIDs.find(musicID) == slot.MusicIDs.end());
    }

}
