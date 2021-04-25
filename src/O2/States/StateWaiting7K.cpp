#include <O2/States/StateWaiting7K.hpp>

#include <Genode/UI/Button.hpp>

#include <O2/States/StatePlanet.hpp>
#include <O2/States/StateRoom.hpp>
#include <O2/Data/Room.hpp>

#include <O2/Character/Avatar.hpp>
#include <O2/Character/ItemFactory.hpp>

void StateWaiting7K::Initialize()
{
    State::Initialize();

    auto& app   = GetApplication();
    auto& items = app.Require<ItemFactory>();
    auto& mixer = app.Require<Gx::Mixer>();

    auto background = Create<Gx::Sprite>("Interface/Metadata/State/Waiting/Background.json");
    AddChild(background);

    auto btnBack = Create<Gx::Button>("Interface/Metadata/State/Waiting/Btn_Back.json");
    btnBack->SetClickCallback([&] (auto& sender, auto& ev)
    {
        mixer.StopAll();
        QueueSceneEvent([=] () { GetDirector().SetScene(new StateRoom(Planet::MusicHall::Kaliope, Planet::ChannelInfo{1})); });
    });

    AddChild(btnBack);

    auto player = Room::PlayerInfo{1, 100, "CXO2", false, Character::Gender::Male};
    auto avatar = Create<Avatar>("Interface/Metadata/State/Waiting/Avatar.json");
    avatar->SetPlayerInfo(player);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    AddChild(avatar);

    auto bgm = mixer.Create<sf::Music>("Interface/Metadata/State/Waiting/Music.json");
    mixer.Play(bgm, "BGM");
}
