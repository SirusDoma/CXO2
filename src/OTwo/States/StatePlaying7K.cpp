#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Core/LifeSystem.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/Tasks/Step.hpp>
#include <Genode/UI.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

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
    m_self(),
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
    m_self(),
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

    auto& session = Require<SessionContext>();
    auto& room    = session.GetCurrentRoom();
    auto& items   = Require<ItemFactory>();


    // IMPORTANT: Don't use callback of these systems, it is being used by chart renderer
    // TODO: Implement multiple listeners for the callback
    const auto scoreTracker = &Require<ScoreTracker>();
    const auto lifeSystem   = &Require<LifeSystem>();
    scoreTracker->Initialize(m_context->GetDifficulty());
    lifeSystem->Initialize(m_context->GetDifficulty());

    const auto jam = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    jam->SetValue(0);

    const auto avatarList = Instantiate<Gx::List>("IDC_LIST_AVATAR");
    const auto avaContainers = avatarList->GetChildren();
    for (int i = 0; i < avaContainers.size(); i++)
    {
        const auto container  = avaContainers[i];
        const auto renderable = dynamic_cast<Gx::Renderable*>(container);
        if (!renderable)
            continue;

        auto member = room.Members[i];
        renderable->SetVisible(member.ID != 0);

        if (!renderable->IsVisible())
            continue;

        const auto avatar = container->FindChild<Avatar>("IDC_AVATAR");
        for (const auto id : member.EquippedItemIDs)
            avatar->Equip(items.GetItem(id));

        const auto info = avatar->GetAvatarInfo();
        info->SetMember(member);

        const auto playerLifeBar = info->GetLifeBar();
        playerLifeBar->SetMaximumValue(lifeSystem->GetMaxLifePoint());
        playerLifeBar->SetValue(lifeSystem->GetMaxLifePoint());

        if (session.GetCurrentPlayer().ID == member.ID)
            m_self = avatar;

        m_avatars[i] = avatar;
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

    m_renderer.SetName("IDC_CHART_RENDERER");
    AddChild(&m_renderer);
    m_renderer.Initialize(*m_context->GetChart(), *m_context, [this, lifeSystem] () {
        CaptureScreen();
        GetDirector().Present<StateResult>();
    });

    // IMPORTANT: All elements after this point need to be called after `ChartRenderer.Render` in order to preserve correct layout ordering

    // Setup Play Menu
    const auto playMenu = Create<PlayMenu>();
    playMenu->SetName("IDC_PLAY_MENU");
    AddChild(playMenu);
    playMenu->Initialize();
    playMenu->SetMetadata(m_context->GetChart()->GetMetadata().ToChartMetadataView(m_context->GetDifficulty()), m_context->GetDifficulty());
    playMenu->SetScoreTracker(*scoreTracker);

    // Setup Score Counter
    const auto scoreNumber = Instantiate<Gx::Number>("IDC_NUMBER_POINT_NUMBER");
    const auto jamGauge = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    const auto bufferContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_BUFFER");
    const auto buffers = bufferContainer->GetChildren();
    for (int i = 0; i < buffers.size(); i++)
    {
        const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
        renderable->SetVisible(false);
    }

    // Setup Life Bar
    const auto lifeBar = Instantiate<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetSlanted(true);
    lifeBar->SetMaximumValue(lifeSystem->GetMaxLifePoint());
    lifeBar->SetValue(0);

    // Setup Jam Combo
    const auto jamContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_NOTE_JAM");
    jamContainer->SetVisible(false);
    const auto jamAnimation = jamContainer->FindChild<Gx::Animation>("IDC_ANIMATION_NOTE_JAM");
    const auto jamNumber    = jamContainer->FindChild<Gx::Number>("IDC_NUMBER_NOTE_JAM");

    jamContainer->SetVisible(false);
    jamAnimation->Stop();
    jamAnimation->SetAnimationCallback([=] (auto animation) {
        jamContainer->SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
    });

    // Setup Combo Counter
    const auto comboCounter = Create<ComboCounter>();
    comboCounter->SetName("IDC_CONTAINER_COMBO");
    AddChild(comboCounter);
    comboCounter->Initialize();

    // Setup Judgement Indicator
    const auto judgementIndicator = Create<JudgementIndicator>();
    judgementIndicator->SetName("IDC_NOTE_JUDGEMENT_INDICATOR");
    AddChild(judgementIndicator);
    judgementIndicator->Initialize();

    m_renderer.SetIncrementCallback([=] (auto& ev, auto acc, auto jamCombo)
    {
        lifeBar->SetValue(lifeSystem->GetCurrentLifePoint());
        m_self->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

        if (lifeSystem->GetCurrentLifePoint() == 0)
        {
            if (m_context->GetDifficulty() != Difficulty::EX && m_self->IsAlive())
            {
                Run(Create<Gx::Delay>(sf::milliseconds(2000),
                    [this] {
                        CaptureScreen();
                        GetDirector().Present<StateResult>();
                    })
                );
            }

            scoreTracker->SetEnabled(false);
            m_self->Die();

            return;
        }

        comboCounter->SetCombo(scoreTracker->GetCombo());
        judgementIndicator->Play(acc);

        scoreNumber->SetValue(scoreTracker->GetScore());
        for (int i = 0; i < buffers.size(); i++)
        {
            const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
            renderable->SetVisible(i < scoreTracker->GetBufferCount());
        }

        jamGauge->SetValue(scoreTracker->GetJamProgress());
    });

    m_renderer.SetJamComboCallback([=] (auto& ev, auto acc, auto jamCombo)
    {
        jamNumber->SetValue(jamCombo);
        jamAnimation->Reset();
        jamContainer->SetVisible(true);
    });

    const auto exitButton = Instantiate<Gx::Button>("IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([this] (const auto &sender, const auto &ev)
    {
        GetDirector().Present<StateWaiting7K>();
    });

    Run(Create<Gx::Step>
    (
        sf::seconds((lifeSystem->GetMaxLifePoint() / (lifeSystem->GetMaxLifePoint() * 0.01f)) * (1.f / 60.f)),
        sf::seconds(1.f / 60.f),
        [this, lifeSystem, lifeBar] (const Gx::Step* step, auto const delta)
        {
            lifeBar->SetValue(lifeBar->GetValue() + static_cast<int>(lifeSystem->GetMaxLifePoint() * 0.01f));
            for (auto [_, avatar] : m_avatars)
                avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

            if (step->GetState() == Gx::TaskState::Completed)
            {
                lifeBar->SetValue(lifeSystem->GetMaxLifePoint());
                for (auto [_, avatar] : m_avatars)
                    avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

                m_renderer.StartRender();
            }
        }
    ));
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

void StatePlaying7K::CaptureScreen()
{
    const auto resources = &GetResources(ResourceScope::Shared);
    const sf::RenderTarget& window = GetApplication();

    if (auto target = sf::RenderTexture(); target.create(window.getSize()))
    {
        target.clear(GetApplication().GetClearColor());
        {
            auto surface = Gx::RenderTargetAdapter(target);

            Update(0);
            surface.Render(*this, Gx::RenderStates::Default);
        }
        target.display();

        auto texture = std::make_unique<sf::Texture>(target.getTexture());
        resources->Store<sf::Texture>("IDC_TEXTURE_STATE_PLAYING", std::move(texture), Gx::CacheMode::Update);
    }
}
