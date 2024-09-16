#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateMyRoom.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>

StateItemShop::StateItemShop(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_items(items)
{
}

void StateItemShop::Initialize()
{
    State::Initialize();

    auto& player   = m_session.GetCurrentPlayer();
    const auto bgm = Instantiate<sf::Music>("BGM/bgItemShop.ogg");

    const auto myRoomButton = Instantiate<Gx::Button>("IDC_BUTTON_MY_ROOM");
    myRoomButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateMyRoom>();
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}