#include <OTwo/States/StateResult.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlaying7K.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>
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

    const auto btnBack = bottom->FindChild<Gx::Button>("IDC_BUTTON_BACK");
    btnBack->SetVisible(false);
    btnBack->SetEnabled(false);
    btnBack->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateWaiting7K>();
    });

    const auto btnRetry = bottom->FindChild<Gx::Button>("IDC_BUTTON_PLAY_RETRY");
    btnRetry->SetVisible(true);
    btnRetry->SetEnabled(false);
    btnRetry->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateLoading>();

        // const auto& config  = Require<GameConfig>();
        // auto ctx = PlayingResourceContext();
        // ctx.SetFxEnabled(config.UseFx);
        // ctx.SetMapID(room.MapID);
        // ctx.SetEffectID(room.EffectID);
        //
        // GetDirector().Present<StatePlaying7K>(ctx);
    });

    const auto topFx    = Create<Gx::Move>(top, sf::Vector2f(0, 0), sf::seconds(2.f));
    const auto bottomFx = Create<Gx::Sequence>([=]
        {
            btnBack->SetVisible(true);
            btnBack->SetEnabled(true);
            btnRetry->SetEnabled(true);
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
