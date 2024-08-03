#include <OTwo/States/StateTest.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Graphics/SpriteBatch.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/List.hpp>

#include <OTwo/States/StateAvi.hpp>

StateTest::StateTest(State &&state) :
    State(std::move(state))
{
}

void StateTest::Initialize()
{
    State::Initialize();

    auto wave = Instantiate<Gx::Gauge>("IDC_GAUGE_WAVE");
    wave->SetValue(50);

    auto jam = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(50);

    auto lifeBar = Instantiate<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetValue(75);

    auto menu = Instantiate<Gx::Image>("IDC_IMAGE_PLAYING_MENU");
    auto bgmVolBar = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_MUSIC");
    bgmVolBar->SetValue(100);

    auto sfxVolBar = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_EFFECT");
    sfxVolBar->SetValue(100);

    // auto list = Load<Gx::List>("IDC_LIST_AVATAR");
    // RemoveChild(list);
    //
    // auto batch = Create<Gx::SpriteBatch>();
    // batch->SetBatchMode(Gx::SpriteBatch::BatchMode::LayerSort);
    // batch->AddChild(list);
    // AddChild(batch);

    // auto& director  = GetDirector();
    // auto background = Load<Gx::Sprite>("State_Test/background");
    //
    // auto sequence = Create<Gx::Sequence>([&]
    //     {
    //         director.Present<StateAvi>();
    //     },
    //     Gx::Sequence::ListOf({
    //         Create<Gx::Fade>(background, 255, sf::seconds(2.25f)),
    //         Create<Gx::Fade>(background, 000, sf::seconds(2.25f))
    //     })
    // );
    // Run(sequence);
}

void StateTest::OnKeyDown(const sf::Event::KeyEvent ev)
{
    State::OnKeyDown(ev);

    if (ev.scancode == sf::Keyboard::Scancode::Space)
        GetDirector().Present<StateTest>();
}

bool StateTest::Close(bool quit)
{
    return true;
}
