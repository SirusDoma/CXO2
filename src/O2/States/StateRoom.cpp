#include <O2/States/StateRoom.hpp>

#include <Genode/Tasks.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StatePlanet.hpp>
#include <O2/States/Components/Room/Marquee.hpp>

#include <O2/Character/ItemFactory.hpp>

#include <iostream>

StateRoom::StateRoom(Planet::MusicHall hall, Planet::ChannelInfo channel) :
    State::State(),
    m_hall(hall),
    m_channel(channel),
    m_optionDialog(),
    m_roomList(),
    m_chatPanel(),
    m_userList()
{
}

void StateRoom::Initialize()
{
    State::Initialize();

    auto background = Create<Gx::Sprite>("Interface/Metadata/State/Room/Background.json");
    AddChild(background);

    auto channelCategory = Create<Gx::Image>("Interface/Metadata/State/Room/ChannelCategory.json");
    switch (m_hall)
    {
        case Planet::MusicHall::Kaliope:  channelCategory->SetFrame("Kaliope");  break;
        case Planet::MusicHall::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case Planet::MusicHall::Philix:   channelCategory->SetFrame("Philix");   break;
        case Planet::MusicHall::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case Planet::MusicHall::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case Planet::MusicHall::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }
    AddChild(channelCategory);

    auto channelNumber = Create<Gx::Number>("Interface/Metadata/State/Room/ChannelNumber.json");
    channelNumber->SetValue(m_channel.Number);
    channelNumber->SetDigitCount(2);
    AddChild(channelNumber);

    auto marquee = Create<Marquee>("Interface/Metadata/State/Room/Marquee.json");
    marquee->SetString("Welcome to O2Jam! Let's play together~");
    AddChild(marquee);

    m_optionDialog = Create<OptionDialog>("Interface/Metadata/Dialog/Option.json");
    m_optionDialog->Initialize(*this);

    auto btnMusicShop = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_MusicShop.json");
    auto btnItemShop  = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_ItemShop.json");
    auto btnMyRoom    = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_MyRoom.json");
    auto btnCoupon    = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_Coupon.json");
    auto btnFirstStep = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_FirstStep.json");
    auto btnOption    = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_Option.json");

    btnOption->SetClickCallback([&] (auto &sender, auto &ev) { m_optionDialog->Show(this, "", false); });

    AddChild(btnMusicShop, btnItemShop, btnMyRoom, btnCoupon, btnFirstStep, btnOption);

    auto nicknameLabel = Create<Gx::Label>("Interface/Metadata/State/Room/NicknameLabel.json");
    nicknameLabel->SetString("Lv.-1: CXO2");
    AddChild(nicknameLabel);

    m_roomList.Initialize(*this);
    Room::RoomData rooms[] = {
        Room::RoomData{
            0,
            "Let's play together~",
            ChartMetadata{"Earth Quake", "Kaze.o2SE", "Kaze.o2SE", "Rock", 36},
            Difficulty::Hard,
            Room::GameMode::Vs,
            Room::SongMode::Normal,
            Room::RoomState::Playing,
            4.5f,
            false,
            2
        },
        Room::RoomData{
            5,
            "Pimplex's room",
            ChartMetadata{},
            Difficulty::Hard,
            Room::GameMode::Single,
            Room::SongMode::Random,
            Room::RoomState::Waiting,
            3.5f,
            false,
        },
        Room::RoomData{
            3,
            "kYo-Abhiem's room",
            ChartMetadata{"R3", "Kaze.o2SE", "Kaze.o2SE", "Rock", 32},
            Difficulty::Hard,
            Room::GameMode::Vs,
            Room::SongMode::Normal,
            Room::RoomState::Waiting,
            4.f,
            false,
        }
    };

    for (auto room : rooms)
        m_roomList.PushRoomData(room);

    AddChild(&m_roomList);

    m_userList.Initialize(*this);
    Room::PlayerInfo players[] = {
        Room::PlayerInfo{1, 100, "CXO2"},
        Room::PlayerInfo{2, 100, "DJZMO"},
        Room::PlayerInfo{3, 98, "kYo-Abhiem"},
        Room::PlayerInfo{4, 53, "Pimplex"}
    };

    for (auto player : players)
        m_userList.AddPlayer(player);

    for (unsigned int i = 21; i > 4; i--)
        m_userList.AddPlayer(Room::PlayerInfo{i, static_cast<int>(i), "Dummy"});

    AddChild(&m_userList);

    m_chatPanel.Initialize(*this);
    AddChild(&m_chatPanel);

    auto btnBack = Create<Gx::Button>("Interface/Metadata/State/Room/Btn_Back.json");
    btnBack->SetClickCallback([this] (auto& sender, auto& ev) { OnExitPlanet(); });
    AddChild(btnBack);

    auto player = players[0];
    player.Gender = Character::Gender::Male;
    m_avatar = Create<Avatar>("Interface/Metadata/State/Room/Avatar.json");
    m_avatar->SetPlayerInfo(player);
    AddChild(m_avatar);

    m_bgm = Create<sf::Music>("Interface/Metadata/State/Room/Music.json", Gx::ResourceScope::Shared);
    Play(m_bgm);
}

void StateRoom::OnExitPlanet()
{
    QueueSceneEvent([this] { GetDirector().SetScene(new StatePlanet(false)); });
}

void StateRoom::Update(double delta)
{
    Scene::Update(delta);
}

sf::RenderStates StateRoom::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states = Scene::Render(target, states);
    return states;
}
