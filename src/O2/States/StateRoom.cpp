#include <O2/States/StateRoom.hpp>

#include <Genode/Tasks.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StatePlanet.hpp>
#include <O2/States/Components/Room/Marquee.hpp>

#include <iostream>

StateRoom::StateRoom(Planet planet, ChannelInfo channel) :
    State::State(),
    m_planet(planet),
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

    auto background = Create<Gx::Sprite>("Metadata/State/Room/Background.json");
    AddChild(background);

    auto channelCategory = Create<Gx::Image>("Metadata/State/Room/ChannelCategory.json");
    switch (m_planet)
    {
        case Planet::Kaliope:  channelCategory->SetFrame("Kaliope");  break;
        case Planet::Kleo:     channelCategory->SetFrame("Kleo");     break;
        case Planet::Philix:   channelCategory->SetFrame("Philix");   break;
        case Planet::Melpomin: channelCategory->SetFrame("Melpomin"); break;
        case Planet::Thalo:    channelCategory->SetFrame("Thalo");    break;
        case Planet::Euta:     channelCategory->SetFrame("Euta");     break;
        default: break;
    }
    AddChild(channelCategory);

    auto channelNumber = Create<Gx::Number>("Metadata/State/Room/ChannelNumber.json");
    channelNumber->SetValue(m_channel.Number);
    channelNumber->SetDigitCount(2);
    AddChild(channelNumber);

    auto marquee = Create<Marquee>("Metadata/State/Room/Marquee.json");
    marquee->SetString("Welcome to O2Jam (Live?). Official Website: https://live.o2jam.asia");
    AddChild(marquee);

    m_optionDialog = Create<OptionDialog>("Metadata/Dialog/Option.json");
    m_optionDialog->Initialize(*this);

    auto btnMusicShop = Create<Gx::Button>("Metadata/State/Room/Btn_MusicShop.json");
    auto btnItemShop  = Create<Gx::Button>("Metadata/State/Room/Btn_ItemShop.json");
    auto btnMyRoom    = Create<Gx::Button>("Metadata/State/Room/Btn_MyRoom.json");
    auto btnCoupon    = Create<Gx::Button>("Metadata/State/Room/Btn_Coupon.json");
    auto btnFirstStep = Create<Gx::Button>("Metadata/State/Room/Btn_FirstStep.json");
    auto btnOption    = Create<Gx::Button>("Metadata/State/Room/Btn_Option.json");

    btnOption->SetClickCallback([&] (auto &sender, auto &ev) { m_optionDialog->Show(this); });

    AddChild(btnMusicShop, btnItemShop, btnMyRoom, btnCoupon, btnFirstStep, btnOption);

    auto nicknameLabel = Create<Gx::Label>("Metadata/State/Room/NicknameLabel.json");
    nicknameLabel->SetString("Lv.-1: CXO2");
    AddChild(nicknameLabel);

    m_roomList.Initialize(*this);
    RoomData rooms[] = {
        RoomData{
            0,
            "Let's play together~",
            ChartMetadata{"Earth Quake", "Kaze.o2SE", "Kaze.o2SE", "Rock", 36},
            Difficulty::Hard,
            GameMode::Vs,
            SongMode::User,
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
            GameMode::Vs,
            SongMode::User,
            RoomState::Waiting,
            4.f,
            false,
        }
    };

    for (auto room : rooms)
        m_roomList.PushRoomData(room);

    AddChild(&m_roomList);

    m_userList.Initialize(*this);
    PlayerInfo players[] = {
        PlayerInfo{1, 100, "CXO2"},
        PlayerInfo{2, 100, "DJZMO"},
        PlayerInfo{3, 98, "kYo-Abhiem"},
        PlayerInfo{4, 53, "Pimplex"}
    };

    for (auto player : players)
        m_userList.AddPlayer(player);

    for (unsigned int i = 21; i > 4; i--)
        m_userList.AddPlayer(PlayerInfo{i, static_cast<int>(i), "Dummy"});

    AddChild(&m_userList);

    m_chatPanel.Initialize(*this);
    AddChild(&m_chatPanel);

    auto btnBack = Create<Gx::Button>("Metadata/State/Room/Btn_Back.json");
    btnBack->SetClickCallback([this] (auto& sender, auto& ev) { OnExitPlanet(); });
    AddChild(btnBack);

    m_bgm = Create<sf::Music>("Metadata/State/Room/Music.json", Gx::ResourceScope::Shared);
    //Play(m_bgm);
}

void StateRoom::OnExitPlanet()
{
    QueueSceneEvent([this] { GetDirector().SetScene(new StatePlanet(false)); });
}
