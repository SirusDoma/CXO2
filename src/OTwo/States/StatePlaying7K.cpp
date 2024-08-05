#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Chart/ChartRenderer.hpp>

#include <Genode/UI.hpp>
#include <Genode/Utilities/Debugger.hpp>

StatePlaying7K::StatePlaying7K() :
    m_renderer(*this, {
        Chart::Channel::Note1,
        Chart::Channel::Note2,
        Chart::Channel::Note3,
        Chart::Channel::Note4,
        Chart::Channel::Note5,
        Chart::Channel::Note6,
        Chart::Channel::Note7
    }),
    m_context(),
    m_config(),
    m_viewport()
{
}

StatePlaying7K::StatePlaying7K(State &&state) :
    State(std::move(state)),
    m_renderer(*this, {
        Chart::Channel::Note1,
        Chart::Channel::Note2,
        Chart::Channel::Note3,
        Chart::Channel::Note4,
        Chart::Channel::Note5,
        Chart::Channel::Note6,
        Chart::Channel::Note7
    }),
    m_context(),
    m_config(),
    m_viewport()
{
}

void StatePlaying7K::Initialize()
{
    State::Initialize();

    auto &app = GetApplication();
    m_context = PrepareContext();
    m_config  = &Require<GameConfig>();

    const auto &session = Require<SessionContext>();
    auto &room = session.GetCurrentRoom();

    if (!m_context->GetChart())
    {
        if (Gx::Debugger::IsDebuggerAttached())
            GetDirector().Present<StateWaiting7K>();
        else
            throw Gx::Exception("Chart cannot be null");

        return;
    }

    const auto metadata = m_context->GetChart()->GetMetadata().ToChartMetadataView(m_context->GetDifficulty());

    const auto wave = Instantiate<Gx::Gauge>("IDC_GAUGE_WAVE");
    wave->SetValue(50);

    const auto jam = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(50);

    const auto lifeBar = Instantiate<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetValue(75);

    const auto menu = Instantiate<Gx::Image>("IDC_IMAGE_PLAYING_MENU");
    const auto title = menu->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE");
    title->SetString(metadata.Title);

    const auto level = menu->FindChild<Gx::Image>("IDC_IMAGE_MUSIC_LEVEL");
    level->SetFrame(room.GetRoomLevelCode(true));

    const auto bgmVolBar = menu->FindChild<Gx::Gauge>( "IDC_GAUGE_VOLUME_MUSIC");
    bgmVolBar->SetValue(100);

    const auto sfxVolBar = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_EFFECT");
    sfxVolBar->SetValue(100);

    const auto noteClickList = Instantiate<Gx::UiContainer>("IDC_LIST_NOTE_CLICK");
    const auto keyEffectContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT");
    const auto keyDownContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_DOWN");
    for (auto [channel, _] : m_config->KeyBindings.at(KeyMode::Seven))
    {
        const int id = static_cast<int>(channel) - 1;
        if (id < 1 || id > 7)
            continue;

        const auto noteClick = noteClickList->FindChild<Gx::Animation>("IDC_ANIMATION_NOTE_CLICK" + std::to_string(id));
        noteClick->SetVisible(false);
        noteClick->Stop();
        noteClick->SetAnimationCallback([] (auto &animation) {
            animation.SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
        });

        const auto keyDown = keyDownContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_DOWN" + std::to_string(id));
        keyDown->SetVisible(false);

        const auto keyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_EFFECT" + std::to_string(id));
        keyEffect->SetFrame(id - 1);
        keyEffect->SetVisible(false);

        m_noteClicks[channel] = noteClick;
        m_keyDowns[channel]   = keyDown;
        m_keyEffects[channel] = keyEffect;
    }

    const auto exitButton = Instantiate<Gx::Button>("IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([this] (const auto &sender, const auto &ev)
    {
        GetDirector().Present<StateWaiting7K>();
    });

    m_renderer.Render(*m_context->GetChart(), *m_context);
}

unsigned int StatePlaying7K::GetViewport() const
{
    return m_viewport;
}

void StatePlaying7K::SetViewport(const unsigned int viewport)
{
    m_viewport = viewport;
}

void StatePlaying7K::Update(const double delta)
{
    State::Update(delta);

    if (!m_context->GetChart())
        return;
}

void StatePlaying7K::OnKeyDown(const sf::Event::KeyEvent ev)
{
    Inputable::OnKeyDown(ev);

    for (auto [channel, code] : m_config->KeyBindings.at(KeyMode::Seven))
    {
        if (code != ev.code)
            continue;

        if (const auto noteClick = m_noteClicks.find(channel); noteClick != m_noteClicks.end())
            noteClick->second->Reset();

        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(true);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(true);

        break;
    }
}

void StatePlaying7K::OnKeyUp(const sf::Event::KeyEvent ev)
{
    Inputable::OnKeyUp(ev);

    if (ev.code == sf::Keyboard::Key::F5)
    {
        if (m_config->NoteShapeType == NoteShape::Square)
            m_config->NoteShapeType = NoteShape::Circle;
        else
            m_config->NoteShapeType = NoteShape::Square;
    }

    for (auto [channel, code] : m_config->KeyBindings.at(KeyMode::Seven))
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

Gx::RenderStates StatePlaying7K::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    m_renderer.Render(surface, states);
    return State::Render(surface, states);
}

const GameContext *StatePlaying7K::PrepareContext() const
{
    const auto context = &Require<GameContext>();
    context->SetViewport(GetViewport());

    if (!context->GetConfig())
        context->SetConfig(Require<GameConfig>());

    return context;
}
