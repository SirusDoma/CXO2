#include <OTwo/States/StateTest.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Gauge.hpp>

#include <OTwo/States/StateAvi.hpp>

StateTest::StateTest(State &state) :
    State::State(state)
{
}

void StateTest::Initialize()
{
    State::Initialize();

    auto wave = Load<Gx::Gauge>("STATE_TEST/IDC_GAUGE_WAVE");
    wave->SetValue(50);

    auto jam = Load<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(50);

    //
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

bool StateTest::Close(bool quit)
{
    return true;
}
