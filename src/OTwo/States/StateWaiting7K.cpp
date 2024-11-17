#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateLoading.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/Models/Room.hpp>

#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>
#include <OTwo/UI/Waiting/MapSelector.hpp>
#include <OTwo/UI/Waiting/InstrumentSelector.hpp>
#include <OTwo/UI/Dialogs/SelectMusicDialog.hpp>

#include <Genode/UI.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <magic_enum.hpp>

StateWaiting7K::StateWaiting7K(Gx::Mixer& mixer, SessionContext& session, GameContext& game, ItemFactory& items) :
    m_mixer(mixer),
    m_game(game),
    m_session(session),
    m_items(items),
    m_playerAvatar(nullptr),
    m_avatars()
{
}

void StateWaiting7K::Initialize()
{
    State::Initialize();

    auto& director  = GetDirector();
    auto& player    = m_session.GetCurrentPlayer();
    auto& room      = m_session.GetCurrentRoom();

    const auto bgm            = Instantiate<sf::Music>("BGM/bgWaiting.ogg");
    const auto sfxStart       = Instantiate<sf::Sound>("bgEffect/33");
    const auto sfxTeam        = Instantiate<sf::Sound>("bgEffect/34");
    const auto sfxSelectMusic = Instantiate<sf::Sound>("bgEffect/35");

    const auto channelCategory = Instantiate<Gx::Image>("IDC_IMAGE_CHANNEL_CATEGORY");
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

    const auto channelNumber = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CHANNEL_ID");
    channelNumber->SetValue(m_session.GetChannelID());

    const auto roomNumber = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_ROOM_ID");
    roomNumber->SetValue(room.ID);

    const auto roomName = Instantiate<Gx::Label>("IDC_TEXT_ROOM_NAME");
    roomName->SetString(room.Title);

    const auto musicName = Instantiate<Gx::Label>("IDC_TEXT_MUSIC_NAME");
    musicName->SetString(room.ChartMetadata.Title + " [BPM: " + Gx::StringHelper::ToString(room.ChartMetadata.BPM, 2) + "]");

    const auto level = Instantiate<Gx::Image>("IDC_IMAGE_ROOM_LEVEL");
    level->SetFrame(room.GetRoomLevelCode());

    if (const auto dialog = Instantiate<Gx::Dialog>("IDC_DIALOG_EMOTICON"); dialog)
    {
        const auto content     = dialog->FindChild<Gx::Image>("IDC_IMAGE_CONTENT");
        const auto currentPage = dialog->FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_PAGE");
        const auto maxPage     = dialog->FindChild<Gx::BitmapNumber>("IDC_NUMBER_MAX_PAGE");
        const auto prevButton  = dialog->FindChild<Gx::Button>("IDC_BUTTON_LEFT");
        const auto nextButton  = dialog->FindChild<Gx::Button>("IDC_BUTTON_RIGHT");

        content->SetFrame(0);
        currentPage->SetValue(1);
        maxPage->SetValue(content->GetFrameCount());

        prevButton->SetClickCallback([=] (auto& sender, auto& ev)
        {
            if (content->GetCurrentFrameIndex() > 0)
            {
                content->SetFrame(content->GetCurrentFrameIndex() - 1);
                currentPage->SetValue(currentPage->GetValue() - 1);
            }
        });

        nextButton->SetClickCallback([=] (auto& sender, auto& ev)
        {
            if (content->GetCurrentFrameIndex() < content->GetFrameCount() - 1)
            {
                content->SetFrame(content->GetCurrentFrameIndex() + 1);
                currentPage->SetValue(currentPage->GetValue() + 1);
            }
        });

        const auto emoticonHelpButton = Instantiate<Gx::Button>("IDC_BUTTON_EMOTICON");
        emoticonHelpButton->SetClickCallback([=] (auto& sender, auto& ev)
        {
            dialog->Show(this, std::string(), false);
        });
    }

    if (const auto dialog = Instantiate<Gx::Dialog>("IDC_DIALOG_CHANGE_TITLE"); dialog)
    {
        const auto titleBox = dialog->FindChild<Gx::InputField>("IDC_EDIT_TITLE");
        titleBox->SetMaximumTextLength(21);
        dialog->SetAcceptCallback([=, r = &room, s = &m_session]
        {
            if (titleBox->GetString().isEmpty())
                return;

            auto data = Room(*r);
            data.Title = titleBox->GetString();

            s->SetCurrentRoom(data);
            roomName->SetString(data.Title);
        });

        const auto changeTitleButton = Instantiate<Gx::Button>("IDC_BUTTON_CHANGE_TITLE");
        changeTitleButton->SetClickCallback([=, r = &room, s = &m_session] (auto& sender, auto& ev)
        {
            if (r->RoomMasterID != s->GetCurrentPlayer().ID)
            {
                ShowDialog("Only Room master can change the room title.", DialogStyle::Information, false, [] (auto _) {});
                return;
            }

            dialog->Show(this, "Please enter a room name.", false);
            changeTitleButton->SetFocus(false);

            titleBox->SetString(roomName->GetString());
            titleBox->SetFocus(true);
            titleBox->SelectAll();
        });
    }

    const auto avatarList = Instantiate<Gx::List>("IDC_LIST_AVATAR");

    AvatarInfo* currentAvatarInfo = nullptr;
    auto currentMember = RoomMember();
    int memberIndex = 0;

    for (const auto child : avatarList->GetChildren())
    {
        if (memberIndex >= sizeof(room.Members) / sizeof(RoomMember))
            break;

        const auto avatar = dynamic_cast<Avatar*>(child);
        if (!avatar)
            continue;

        m_avatars.push_back(avatar);
        auto& emoticonContainer = Create<Gx::UiContainer>();
        emoticonContainer.SetName("IDC_CONTAINER_EMOTICON");
        emoticonContainer.SetVisible(false);
        avatar->AddChild(emoticonContainer);

        const auto avatarInfo = avatar->FindChild<AvatarInfo>("IDC_AVATAR_INFO");
        auto& member = room.Members[memberIndex];
        if (member.ID == 0)
        {
            avatar->SetVisible(false);
            avatar->ClearEquipments();
            avatarInfo->Reset();

            memberIndex++;
            continue;
        }

        if (member.ID == m_session.GetCurrentPlayer().ID)
        {
            currentMember = member;
            currentAvatarInfo = avatarInfo;
            m_playerAvatar = avatar;
        }

        const auto bossMark = avatar->FindChild<Gx::Sprite>("IDC_IMAGE_BOSS_MARK");
        const auto noMusic  = avatar->FindChild<Gx::Sprite>("IDC_IMAGE_NO_MUSIC");

        bossMark->SetVisible(member.ID == room.RoomMasterID);
        noMusic->SetVisible(false);

        avatar->SetGender(member.Gender);
        avatarInfo->SetMember(const_cast<RoomMember&>(member));

        for (auto [_, item]: m_items.GetDefaultItems(member.Gender))
            avatar->SetDefaultItem(std::move(item));

        for (const auto itemID : member.EquippedItemIDs)
            avatar->Equip(m_items.Create(itemID));

        memberIndex++;
    }

    const auto teamButtons = Instantiate<Gx::UiContainer>("IDC_CONTAINER_TEAM_BUTTONS");
    auto teamButtonMatcher = [=] (const RoomTeam team) -> Gx::RadioButton*
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

        teamButton->SetCheckStateChangeCallback([=] (auto& sender)
        {
            if (!sender.IsChecked())
                return;

            if (currentAvatarInfo)
            {
                if (const auto member = currentAvatarInfo->GetMember(); member)
                    member->Team = team;

                currentAvatarInfo->Invalidate();
                m_mixer.Play(sfxTeam);
            }
        });
    }

    const auto mapSelector = Instantiate<MapSelector>("IDC_CONTAINER_MAP_SELECTOR");
    mapSelector->SetMapChangedCallback([=, s = &m_session, r = &room] (const unsigned int mapID)
    {
        auto data = Room(*r);
        data.MapID = mapID;

        s->SetCurrentRoom(data);
    });

    mapSelector->SetEffectChangedCallback([=, s = &m_session, r = &room] (const unsigned int effectID)
    {
        auto data = Room(*r);
        data.EffectID = effectID;

        s->SetCurrentRoom(data);
    });

    mapSelector->SetMapID(room.MapID, true);
    mapSelector->SetEffectID(room.EffectID);

    const auto instrumentSelector = Instantiate<InstrumentSelector>("IDC_CONTAINER_INSTRUMENT_SELECTOR");
    if (currentAvatarInfo)
    {
        instrumentSelector->SetInstrumentSelectCallack([=, &room] (const ItemMetadata& metadata)
        {
            auto item = m_items.Create(metadata.ID);
            if (const auto avatar = currentAvatarInfo->GetAvatar(); avatar)
            {
                auto data = Room(room);
                if (item.GetID() == 0 || avatar->IsEquiped(item))
                {
                    avatar->Unequip(EquipmentType::Bass);
                    avatar->Unequip(EquipmentType::Guitar);
                    avatar->Unequip(EquipmentType::Keyboard);
                    avatar->Unequip(EquipmentType::Drum);
                }
                else
                    avatar->Equip(std::move(item));

                for (auto& member : data.Members)
                {
                    if (member.ID != currentAvatarInfo->GetMember()->ID)
                        continue;

                    const auto defaultItems = m_items.GetDefaultItems(member.Gender);
                    member.EquippedItemIDs.clear();

                    for (auto [_, equipedItem] : avatar->GetEquipedItems())
                        member.EquippedItemIDs.push_back(equipedItem->GetID());
                }

                m_session.SetCurrentRoom(data);
            }
        });
    }

    for (const auto id : player.Inventory)
        instrumentSelector->AddInstrumentMetadata(m_items.GetItemMetadata(id));

    const auto chatPanel  = Instantiate<ChatPanel>("IDC_CHAT_PANEL");
    chatPanel->SetMaximumTextLength(50);

    const auto chatWindow = chatPanel->GetChatWindow();
    chatWindow->PushSystemMessage("Welcome to O2Jam!");
    chatWindow->PushSystemMessage("Let's play together~");

    if (const auto selectMusicDialog = Instantiate<SelectMusicDialog>("IDC_DIALOG_SELECT_MUSIC"); selectMusicDialog)
    {
        selectMusicDialog->Initialize(); // force load the cover art
        if (const auto selectMusicButton = Instantiate<Gx::Button>("IDC_BUTTON_SELECT_MUSIC"); selectMusicButton)
        {
            selectMusicButton->SetClickCallback([=] (auto& sender, auto& ev)
            {
                m_mixer.Play(sfxSelectMusic);
                selectMusicDialog->Show(this, std::string(), false);
            });
        }

        selectMusicDialog->SetAcceptCallback([=, &room] ()
        {
            auto data = Room(room);
            data.Difficulty = selectMusicDialog->GetSelectedDifficulty();
            data.Speed      = selectMusicDialog->GetSelectedSpeed();

            const auto music  = selectMusicDialog->GetSelectedMusic();
            const auto random = selectMusicDialog->GetSelectedRandomLevels() != static_cast<LevelCategory>(0);
            if (!music.Source.empty())
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

            m_session.SetCurrentRoom(data);
            level->SetFrame(data.GetRoomLevelCode());
        });
    }


    const auto btnBack = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    btnBack->SetClickCallback([&] (auto&, auto&) {
        director.Present<StateRoom>();
    });

    const auto readyButton = Instantiate<Gx::ToggleButton>("IDC_BUTTON_READY");
    readyButton->SetVisible(m_session.GetCurrentPlayer().ID != room.RoomMasterID);
    readyButton->SetEnabled(readyButton->IsVisible());
    readyButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (!sender.IsChecked())
            return;
    });

    const auto btnStart = Instantiate<Gx::ToggleButton>("IDC_BUTTON_START");
    btnStart->SetVisible(m_session.GetCurrentPlayer().ID == room.RoomMasterID);
    btnStart->SetEnabled(btnStart->IsVisible());
    btnStart->SetCheckStateChangeCallback([=, &director] (auto& sender)
    {
        if (!sender.IsChecked())
            return;

        sender.SetEnabled(false);
        btnBack->SetEnabled(false);
        m_mixer.Play(sfxStart, "SFX");

        const auto& data = m_session.GetCurrentRoom();
        if (!m_game.GetChart() || m_game.GetChart()->Source != data.ChartMetadata.Source)
        {
            auto chart    = std::make_unique<Chart>();
            chart->Source = data.ChartMetadata.Source;

            m_game.SetChart(std::move(chart));
        }

        m_game.SetMode(data.GameMode);
        m_game.SetDifficulty(data.Difficulty);
        m_game.SetSpeed(data.Speed);
        m_game.SetMapID(data.MapID);
        m_game.SetEffectID(data.EffectID);

        Run(Create<Gx::Delay>(sf::milliseconds(100.f), [&director]
        {
            director.Present<StateLoading>();
        }));
    });

    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}

void StateWaiting7K::OnKeyPressed(const sf::Event::KeyPressed& ev)
{
    State::OnKeyPressed(ev);

    if (ev.code == sf::Keyboard::Key::F3)
    {
        if (const auto btnStart = Instantiate<Gx::ToggleButton>("IDC_BUTTON_START"))
            btnStart->SetCheckedState(true);
    }

    if (ev.control)
    {
        if (ev.shift)
        {
            switch (ev.code)
            {
                case sf::Keyboard::Key::Num1: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_HI");          break;
                case sf::Keyboard::Key::Num2: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_GO");          break;
                case sf::Keyboard::Key::Num3: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_READY");       break;
                case sf::Keyboard::Key::Num4: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_APPROVAL");    break;
                case sf::Keyboard::Key::Num5: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_OBJECTION");   break;
                case sf::Keyboard::Key::Num6: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_OHM_SMILE");   break;
                case sf::Keyboard::Key::Num7: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_OHM_HAHA");    break;
                case sf::Keyboard::Key::Num8: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_OHM_VICTORY"); break;
                case sf::Keyboard::Key::Num9: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_OHM_LOSE");    break;
                case sf::Keyboard::Key::Num0: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_BY_DEGREES");  break;
                default: break;
            }
        }
        else
        {
            switch (ev.code)
            {
                case sf::Keyboard::Key::Num1: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK");  break;
                case sf::Keyboard::Key::Num2: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_QUESTION");          break;
                case sf::Keyboard::Key::Num3: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_HEART");             break;
                case sf::Keyboard::Key::Num4: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_COUNT");             break;
                case sf::Keyboard::Key::Num5: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_NOTE");              break;
                case sf::Keyboard::Key::Num6: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_EXCLAMATION_MARK2"); break;
                case sf::Keyboard::Key::Num7: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_BROKEN_HEART");      break;
                case sf::Keyboard::Key::Num8: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_CLOVER");            break;
                case sf::Keyboard::Key::Num9: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_FLOWER");            break;
                case sf::Keyboard::Key::Num0: ShowEmoticon(m_playerAvatar, "IDC_ANIMATION_EMOTICON_IDEA");              break;
                default: break;
            }
        }
    }
}

void StateWaiting7K::OnKeyReleased(const sf::Event::KeyReleased& ev)
{
    State::OnKeyReleased(ev);
}

void StateWaiting7K::ShowEmoticon(const Avatar* avatar, const std::string& emoticonID)
{
    const auto container = avatar->FindChild<Gx::UiContainer>("IDC_CONTAINER_EMOTICON");
    if (container->IsVisible())
        return;

    auto emoticon = container->FindChild<Gx::Animation>(emoticonID);
    if (!emoticon)
    {
        emoticon = &Create<Gx::Animation>(*FindResource<Gx::Animation>(emoticonID));
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

