#include <OTwo/States/StateWaiting7K.hpp>

#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Data/Room.hpp>
#include <OTwo/Data/UserState.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>

StateWaiting7K::StateWaiting7K(State &state) :
    State(state)
{
}

void StateWaiting7K::Initialize()
{
    State::Initialize();

    auto& app      = GetApplication();
    auto& director = GetDirector();
    auto& items    = app.Require<ItemFactory>();
    auto& mixer    = app.Require<Gx::Mixer>();
    auto& state    = app.Require<UserState>();

    auto bgm = Load<sf::Music>("STATE_WAITING/IDC_MUSIC");

    auto btnBack = Load<Gx::Button>("STATE_WAITING/IDC_BUTTON_BACK");
    btnBack->SetClickCallback([&] (auto& sender, auto& ev) {
        director.Present<StateRoom>();
    });

    auto avatar = Load<Avatar>("STATE_WAITING/IDC_AVATAR");
    avatar->SetPlayer(state.GetPlayer());
    for (auto [_, item] : items.GetDefaultItems(state.GetPlayer().Gender))
        avatar->SetDefaultItem(item);

    mixer.Play(bgm, "BGM");
}

