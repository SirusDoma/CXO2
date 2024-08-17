#include <OTwo/States/StateResult.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlaying7K.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>
#include <Genode/Fx/Move.hpp>

StateResult::StateResult(State &&state) :
    State(std::move(state))
{
}

void StateResult::Initialize()
{
    State::Initialize();

    auto& mixer         = Require<Gx::Mixer>();
    const auto& session = Require<SessionContext>();
    const auto& room    = session.GetCurrentRoom();

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

    const auto btnRetry = bottom->FindChild<Gx::Button>("IDC_BUTTON_PLAY_RETRY");
    btnRetry->SetEnabled(false);
    btnRetry->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateLoading>();
    });

    const auto btnBack = bottom->FindChild<Gx::Button>("IDC_BUTTON_BACK");
    btnBack->SetEnabled(false);
    btnBack->SetFocusChangedCallback([btnRetry] (auto &sender, const auto &ev)
    {
        btnRetry->SetFocus(sender.IsFocused() ? false : btnRetry->IsFocused());
        btnRetry->SetEnabled(!sender.IsFocused());
    });
    btnBack->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateWaiting7K>();
    });

    const auto topFx    = Create<Gx::Move>(top, sf::Vector2f(0, 0), sf::seconds(2.f));
    const auto bottomFx = Create<Gx::Sequence>([=]
        {
            btnRetry->SetEnabled(true);
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
    Run(Create<Gx::Delay>(sf::seconds(10.f), [=] { btnBack->PerformClick(); }));
}

void StateResult::Update(const double delta)
{
    State::Update(delta);
}

Gx::RenderStates StateResult::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    return State::Render(surface, states);
}

void StateResult::OnKeyDown(const sf::Event::KeyEvent ev)
{
    State::OnKeyDown(ev);

    if (ev.code == sf::Keyboard::Key::Enter)
    {
        if (const auto bottom = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_BOTTOM"); bottom)
        {
            if (const auto btnBack = bottom->FindChild<Gx::Button>("IDC_BUTTON_BACK"); btnBack)
                btnBack->PerformClick();
        }
    }
}
