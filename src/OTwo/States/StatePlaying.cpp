#include <OTwo/States/StatePlaying.hpp>
#include <Genode/UI.hpp>

StatePlaying::StatePlaying(State &state) :
    State(state)
{
}

void StatePlaying::Initialize()
{
    State::Initialize();

    const auto wave = Load<Gx::Gauge>("IDC_GAUGE_WAVE");
    wave->SetValue(50);

    const auto jam = Load<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(50);

    const auto lifeBar = Load<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetValue(75);

    const auto menu = Load<Gx::Image>("IDC_IMAGE_PLAYING_MENU");
    const auto bgmVolBar = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_MUSIC");
    bgmVolBar->SetValue(100);

    const auto sfxVolBar = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_EFFECT");
    sfxVolBar->SetValue(100);
}
