#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Core/ChartRenderer.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <Genode/UI.hpp>
#include <Genode/Utilities/Debugger.hpp>

StatePlaying7K::StatePlaying7K() :
    m_renderer(ChannelSet{
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
    m_renderer(ChannelSet{
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

    m_context = PrepareContext();
    m_config  = &Require<GameConfig>();

    const auto jam = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(0);

    const auto lifeBar = Instantiate<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetValue(100);

    const auto avatarList = Instantiate<Gx::List>("IDC_LIST_AVATAR");
    const auto avaContainers = avatarList->GetChildren();
    unsigned int pIndex = 0;
    for (const auto container : avaContainers)
    {
        const auto renderable = dynamic_cast<Gx::Renderable*>(container);
        if (pIndex != 1)
            renderable->SetVisible(false);

        pIndex++;
    }

    const auto keyEffectContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT");
    const auto keyDownContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_DOWN");
    for (auto [channel, _] : m_config->KeyBindings.at(KeyMode::Seven))
    {
        const int id = static_cast<int>(channel) - 1;
        if (id < 1 || id > 7)
            continue;


        const auto keyDown = keyDownContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_DOWN" + std::to_string(id));
        keyDown->SetVisible(false);

        const auto keyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_EFFECT" + std::to_string(id));
        keyEffect->SetFrame(id - 1);
        keyEffect->SetVisible(false);

        m_keyDowns[channel]   = keyDown;
        m_keyEffects[channel] = keyEffect;
    }

    AddChild(&m_renderer);
    m_renderer.Render(*m_context->GetChart(), *m_context, [this] () {
        const auto resources = &GetResources(ResourceScope::Shared);
        const sf::RenderTarget& target = GetApplication();
        const auto view = target.getView();

        if (auto texture = std::make_unique<sf::Texture>(); texture->create(sf::Vector2u{static_cast<unsigned int>(view.getSize().x), static_cast<unsigned int>(view.getSize().y)}))
        {
            texture->update(GetApplication());
            resources->Store<sf::Texture>("IDC_TEXTURE_STATE_PLAYING", std::move(texture), Gx::CacheMode::Update);
        }

        GetDirector().Present<StateResult>();
    });

    const auto exitButton = Instantiate<Gx::Button>("IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([this] (const auto &sender, const auto &ev)
    {
        GetDirector().Present<StateWaiting7K>();
    });

    // HACK: Make exit button top-level
    RemoveChild(exitButton);
    AddChild(exitButton);
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

        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(true);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(true);

        //m_renderer.Input(channel, true);
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

    if (ev.code == sf::Keyboard::Key::F6)
    {
        if (m_config->NoteGuideLength == 3)
            m_config->NoteGuideLength = 0;
        else
            m_config->NoteGuideLength++;
    }

    for (auto [channel, code] : m_config->KeyBindings.at(KeyMode::Seven))
    {
        if (code != ev.code)
            continue;

        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(false);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(false);

        //m_renderer.Input(channel, false);
        break;
    }
}

Gx::RenderStates StatePlaying7K::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
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
