#include <OTwo/States/StatePlaying.hpp>
#include <Genode/UI.hpp>
#include <OTwo/Config/GameConfig.hpp>

StatePlaying::StatePlaying(State &state) :
    State(state)
{
}

void StatePlaying::Initialize()
{
    State::Initialize();

    auto &app = GetApplication();
    m_config  = &app.GetConfig<GameConfig>();

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

    auto keyEffectContainer = Load<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT");
    for (auto [channel, _] : m_config->SevenKeyBinding)
    {
        const int id = static_cast<int>(channel) - 1;
        if (id < 1 || id > 7)
            continue;

        const auto keyDown = Load<Gx::Image>("IDC_IMAGE_KEY_DOWN" + std::to_string(id));
        keyDown->SetVisible(false);

        const auto keyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_EFFECT" + std::to_string(id));
        keyEffect->SetFrame(id - 1);
        keyEffect->SetVisible(false);

        m_keyDowns[channel]   = keyDown;
        m_keyEffects[channel] = keyEffect;
    }

    // const auto noteClick1 = Load<Gx::Animation>("IDC_ANIMATION_NOTE_CLICK1");
    // noteClick1->SetLoop(true);
    // AddChild(noteClick1);
}

void StatePlaying::Update(const double delta)
{
    State::Update(delta);
}

void StatePlaying::OnKeyDown(const sf::Event::KeyEvent ev)
{
    Inputable::OnKeyDown(ev);

    for (auto [channel, code] : m_config->SevenKeyBinding)
    {
        if (code != ev.code)
            continue;

        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(true);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(true);

        break;
    }
}

void StatePlaying::OnKeyUp(const sf::Event::KeyEvent ev)
{
    Inputable::OnKeyUp(ev);

    for (auto [channel, code] : m_config->SevenKeyBinding)
    {
        if (code != ev.code)
            continue;

        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(false);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(false);

        break;
    }
}
