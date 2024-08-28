#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

#include <Genode/UI/Button.hpp>

StateMyRoom::StateMyRoom(State &&state) :
    State::State(std::move(state))
{
}

void StateMyRoom::Initialize()
{
    const auto& session  = Require<SessionContext>();
    const auto& items    = Require<ItemFactory>();
    const auto player = session.GetCurrentPlayer();

    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    for (const auto id : player.Inventory)
    {
        if (const auto item = items.GetItem(id); item)
            avatar->Equip(item);
    }

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });
}