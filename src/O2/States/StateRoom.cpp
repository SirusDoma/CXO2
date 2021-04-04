#include <O2/States/StateRoom.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StatePlanet.hpp>
#include <O2/States/Components/Room/RoomButton.hpp>

StateRoom::StateRoom(Planet planet, ChannelInfo channel) :
    State::State(),
    m_planet(planet),
    m_channel(channel),
    m_buttons()
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

    auto btnMusicShop = Create<Gx::Button>("Metadata/State/Room/Btn_MusicShop.json");
    auto btnItemShop  = Create<Gx::Button>("Metadata/State/Room/Btn_ItemShop.json");
    auto btnMyRoom    = Create<Gx::Button>("Metadata/State/Room/Btn_MyRoom.json");
    auto btnCoupon    = Create<Gx::Button>("Metadata/State/Room/Btn_Coupon.json");
    auto btnFirstStep = Create<Gx::Button>("Metadata/State/Room/Btn_FirstStep.json");
    auto btnOption    = Create<Gx::Button>("Metadata/State/Room/Btn_Option.json");

    AddChild(btnMusicShop, btnItemShop, btnMyRoom, btnCoupon, btnFirstStep, btnOption);

    auto btnCreateRoom = Create<Gx::Button>("Metadata/State/Room/Btn_CreateRoom.json");
    btnCreateRoom->SetClickCallback([] (auto& sender, auto& event)
    {

    });
    AddChild(btnCreateRoom);

    auto roomList = Create<Gx::Repeater>("Metadata/State/Room/RoomList.json");
    for (unsigned int i = 0; i < 6; i++)
    {
        auto roomButton = std::make_unique<RoomButton>(*this);
        auto roomData = RoomData{
            i + 1,
            "Let's play together~",
            ChartMetadata{"Earth Quake", "Kaze.o2SE", "Kaze.o2SE", "Rock", 32},
            i <= 2 ? Difficulty::Easy : i <= 4 ? Difficulty::Normal : i <= 6 ? Difficulty::Hard : Difficulty::Master,
            SpeedType::HiSpeed,
            i % 2 != 0 ? RoomState::Playing : RoomState::Waiting,
            (i + 1) * 0.5f,
            i % 2 == 0
        };

        roomButton->SetRoomData(roomData);

        roomList->AddChild(roomButton.get());
        m_buttons.push_back(std::move(roomButton));
    }
    AddChild(roomList);

    auto btnShortCut    = Create<Gx::Button>("Metadata/State/Room/Btn_ShortCut.json");
    AddChild(btnShortCut);

    auto btnShowAll     = Create<Gx::Button>("Metadata/State/Room/Btn_ShowAll.json");
    auto btnWaitingRoom = Create<Gx::Button>("Metadata/State/Room/Btn_WaitingRoom.json");
    btnShowAll->SetEnabled(false);
    btnShowAll->SetVisible(false);

    btnShowAll->SetClickCallback([=] (auto& sender, auto& ev)
    {
        btnShowAll->SetEnabled(false);
        btnShowAll->SetVisible(false);
        btnWaitingRoom->SetEnabled(true);
        btnWaitingRoom->SetVisible(true);
    });

    btnWaitingRoom->SetClickCallback([=] (auto& sender, auto& ev)
    {
        btnWaitingRoom->SetEnabled(false);
        btnWaitingRoom->SetVisible(false);
        btnShowAll->SetEnabled(true);
        btnShowAll->SetVisible(true);
    });

    AddChild(btnWaitingRoom, btnShowAll);

    auto btnRoomLeft  = Create<Gx::Button>("Metadata/State/Room/Btn_RoomLeft.json");
    auto btnRoomRight = Create<Gx::Button>("Metadata/State/Room/Btn_RoomRight.json");
    AddChild(btnRoomLeft, btnRoomRight);

    auto btnBack = Create<Gx::Button>("Metadata/State/Room/Btn_Back.json");
    btnBack->SetClickCallback([this] (auto& sender, auto& ev) { OnExitPlanet(); });
    AddChild(btnBack);

    m_bgm = Create<sf::Music>("Metadata/State/Room/Music.json", Gx::ResourceScope::Shared);
    Play(m_bgm);
}

void StateRoom::OnExitPlanet()
{
    QueueSceneEvent([this] { GetDirector().SetScene(new StatePlanet(false)); });
}
