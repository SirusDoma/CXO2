#include <OTwo/States/StateResult.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>
#include <Genode/Fx/Move.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

StateResult::StateResult(State &&state) :
    State(std::move(state))
{
}

void StateResult::Initialize()
{
    State::Initialize();

    auto& mixer   = Require<Gx::Mixer>();
    auto &session = Require<SessionContext>();
    auto &room    = session.GetCurrentRoom();
    auto &game    = Require<GameContext>();

    if (const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_BACKGROUND"); container)
    {
        const auto& resources = GetResources(ResourceScope::Shared);
        if (const auto texture = resources.Find<sf::Texture>("IDC_TEXTURE_STATE_PLAYING"); texture)
        {
            const auto fragment = Create<Gx::Sprite>(*texture);
            container->AddChild(fragment);
        }
    }

    auto const view = GetView();

    auto top = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_TOP");
    auto bottom = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_BOTTOM");

    auto height = bottom->GetLocalBounds().height;
    if (height < top->GetLocalBounds().height)
        height = top->GetLocalBounds().height;

    top->SetPosition(0.f, -height);
    bottom->SetPosition(0.f, view.getSize().y + height);

    const auto btnBack = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    btnBack->SetVisible(false);
    btnBack->SetEnabled(false);
    btnBack->SetClickCallback([this, &mixer] (const auto &sender, const auto &ev)
    {
        mixer.StopAll();
        GetDirector().Present<StateWaiting7K>();
    });


    const auto topFx    = Create<Gx::Move>(top, sf::Vector2f(0, 0), sf::seconds(2.f));
    const auto bottomFx = Create<Gx::Sequence>([=]
        {
            btnBack->SetVisible(true);
            btnBack->SetEnabled(true);
        },
        Gx::Sequence::ListOf(
        {
            Create<Gx::Move>(bottom, sf::Vector2f(0, view.getSize().y - bottom->GetLocalBounds().height), sf::seconds(2.f)),
        })
    );

    if (const auto bgm = Instantiate<sf::Music>("IDC_MUSIC"); bgm)
        mixer.Play(bgm, "BGM");

    Run(topFx, bottomFx);
}

void StateResult::Update(const double delta)
{
    State::Update(delta);
}
