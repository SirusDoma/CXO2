#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Core/LifeSystem.hpp>
#include <OTwo/Core/ScoreTracker.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/UI/Playing/JudgementIndicator.hpp>
#include <OTwo/UI/Playing/PlayMenu.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/Tasks/Step.hpp>
#include <Genode/UI.hpp>
#include <Genode/Utilities/Randomizer.hpp>

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
    m_chatBox(),
    m_viewport()
{
}

void StatePlaying7K::Initialize()
{
    State::Initialize();

    m_context = PrepareContext();
    m_config  = &Require<GameConfig>();

    auto& session     = Require<SessionContext>();
    auto& room        = session.GetCurrentRoom();
    const auto& items = Require<ItemFactory>();

    const auto chatPanel = Instantiate<ChatPanel>("IDC_CHAT_PANEL");
    chatPanel->Initialize();
    chatPanel->SetMaximumTextLength(50);
    m_chatBox = chatPanel->FindChild<Gx::TextBox>("IDC_EDIT_CHAT");
    m_chatBox->SetPermanentFocusEnabled(true);
    m_chatBox->SetEnabled(false);

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
        const auto container = dynamic_cast<Gx::UiContainer*>(avaContainers[i]);
        if (!container)
            continue;

        if (i >= room.MaxCapacity)
            break;

        auto member = room.Members[i];
        container->SetVisible(member.ID != 0);

        if (!container->IsVisible())
            continue;

        const auto avatar = container->FindChild<Avatar>("IDC_AVATAR");
        for (const auto id : member.EquippedItemIDs)
            avatar->Equip(items.GetItem(id));

        const auto effectContainer = Create<Gx::UiContainer>();
        effectContainer->SetName("IDC_CONTAINER_EFFECT_JAM");

        if (const auto fxPrefab = FindResource<Gx::Animation>("IDC_ANIMATION_EFFECT_JAM"); fxPrefab)
        {
            const auto fx = Create<Gx::Animation>(*fxPrefab);
            fx->SetName("IDC_ANIMATION_EFFECT_JAM");
            fx->Stop();
            fx->SetAnimationCallback([=] (auto& _) {
                effectContainer->SetVisible(
                    fx->GetState() == Gx::Animation::AnimationState::Playing ||
                    fx->GetState() == Gx::Animation::AnimationState::Initial
                );
            });

            effectContainer->AddChild(fx);
        }

        if (const auto numPrefab = FindResource<Gx::Number>("IDC_NUMBER_EFFECT_JAM"); numPrefab)
        {
            const auto numEffect = Create<Gx::Number>(*numPrefab);
            numEffect->SetName("IDC_NUMBER_EFFECT_JAM");
            numEffect->SetAnimationCallback([=] (auto& _) {
                numEffect->SetVisible(
                    numEffect->GetAnimationState() == Gx::Animation::AnimationState::Playing ||
                    numEffect->GetAnimationState() == Gx::Animation::AnimationState::Initial
                );
            });
            effectContainer->AddChild(numEffect);
        }

        effectContainer->SetVisible(false);
        avatar->AddChild(effectContainer);

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
    m_renderer.Initialize(*m_context->GetChart(), *m_context, [this] () {
        OnRenderComplete();
    });

    // ----------------------------------------------------------------------------------------------------------------------------------------
    // IMPORTANT: All elements after this point need to be called after `ChartRenderer.Initialize` in order to preserve correct layout ordering.
    //            Every resource below is likely to be a prefab (Resources that are defined under `"require"`).
    //
    //            With this, we don't have to maintain depth position, which SFML does not support natively.
    // ----------------------------------------------------------------------------------------------------------------------------------------

    // Setup Long Note effects
    if (const auto longNoteEffectList = FindResource<Gx::List>("IDC_LIST_LONG_NOTE_EFFECT"); longNoteEffectList)
    {
        for (auto [channel, _] : m_config->KeyBindings.at(KeyMode::Seven))
        {
            const int id = static_cast<int>(channel) - 1;
            if (id < 1 || id > 7)
                continue;

            const auto longNoteEffect = longNoteEffectList->FindChild<Gx::Animation>("IDC_ANIMATION_LONG_NOTE_EFFECT" + std::to_string(id));
            longNoteEffect->SetVisible(false);
            m_longNoteEffects[channel] = longNoteEffect;

            AddChild(longNoteEffect);
        }
    }

    // Setup Note Clicks
    if (const auto noteClickList = FindResource<Gx::List>("IDC_LIST_NOTE_CLICK"); noteClickList)
    {
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

            m_noteClicks[channel] = noteClick;
            AddChild(noteClick);
        }
    }

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
    const auto judgementIndicator = Create<JudgementIndicator>(m_config->UseFx);
    judgementIndicator->SetName("IDC_NOTE_JUDGEMENT_INDICATOR");
    AddChild(judgementIndicator);
    judgementIndicator->Initialize();

    // Setup Key Effects
    m_renderer.SetInputCallback([=] (auto channel, bool state)
    {
        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(state);

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(state);
    });

    // Setup Score changes
    m_renderer.SetIncrementCallback([=] (auto& ev, auto acc, auto jamCombo)
    {
        // Life System
        lifeBar->SetValue(lifeSystem->GetCurrentLifePoint());
        m_self->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());
        if (lifeSystem->GetCurrentLifePoint() == 0)
        {
            if (m_context->GetDifficulty() != Difficulty::EX && m_self->IsAlive())
            {
                Run(Create<Gx::Delay>(sf::milliseconds(2000),
                    [this] {
                        OnRenderComplete();
                    })
                );
            }

            scoreTracker->SetEnabled(false);
            m_self->Die();

            return;
        }

        // Note Click and Long Note Effects
        m_noteClicks[ev.Channel]->Reset();
        m_longNoteEffects[ev.Channel]->SetVisible(false);
        if (acc == Accuracy::Bad || acc == Accuracy::Miss)
        {
            m_noteClicks[ev.Channel]->Stop();
            if (m_config->UseFx)
                m_longNoteEffects[ev.Channel]->SetVisible(false);
        }
        else if (ev.Type == Chart::NoteType::Hold && m_config->UseFx)
        {
            m_longNoteEffects[ev.Channel]->Reset();
            m_longNoteEffects[ev.Channel]->SetVisible(true);
        }

        // Combo
        comboCounter->SetCombo(scoreTracker->GetCombo());
        judgementIndicator->Play(acc);

        // Score and Jam Combo
        scoreNumber->SetValue(scoreTracker->GetScorePoint());
        jamGauge->SetValue(scoreTracker->GetJamProgress());

        // Buffer
        for (int i = 0; i < buffers.size(); i++)
        {
            const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
            renderable->SetVisible(i < scoreTracker->GetBufferCount());
        }
    });

    // Setup jam combo effect
    m_renderer.SetJamComboCallback([=] (auto& ev, auto acc, auto jamCombo)
    {
        jamNumber->SetValue(jamCombo);
        jamAnimation->Reset();
        jamContainer->SetVisible(true);

        const auto effectContainer = m_self->FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_JAM");
        if (!effectContainer)
            return;

        for (const auto child : effectContainer->GetChildren())
        {
            if (const auto number = dynamic_cast<Gx::Number*>(child); number)
            {
                number->SetValue(jamCombo);
                number->Reset();
            }

            if (const auto animation = dynamic_cast<Gx::Animation*>(child); animation)
                animation->Reset();
        }
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
        [this, lifeSystem, lifeBar] (const auto& step, auto const delta)
        {
            lifeBar->SetValue(lifeBar->GetValue() + static_cast<int>(lifeSystem->GetMaxLifePoint() * 0.01f));
            for (auto [_, avatar] : m_avatars)
                avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

            if (step.GetState() == Gx::TaskState::Completed)
            {
                lifeBar->SetValue(lifeSystem->GetMaxLifePoint());

                // TODO: There's no need to animate avatar life bar?
                for (auto [_, avatar] : m_avatars)
                    avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

                m_renderer.StartRender();
            }
        }
    ));
}

void StatePlaying7K::OnRenderComplete()
{
    auto& session = Require<SessionContext>();
    const auto& scoreTracker = Require<ScoreTracker>();

    auto items = std::array<ScoreResultItem, 8>();
    for (int i = 0; i < items.size(); i++)
    {
        auto& member = session.GetCurrentRoom().Members[i];
        if (member.ID == session.GetCurrentPlayer().ID)
        {
            items[i] = ScoreResultItem{
                member,
                scoreTracker.GetPoint(Accuracy::Cool),
                scoreTracker.GetPoint(Accuracy::Good),
                scoreTracker.GetPoint(Accuracy::Bad),
                scoreTracker.GetPoint(Accuracy::Miss),
                scoreTracker.GetMaxCombo(),
                scoreTracker.GetMaxJamCombo(),
                scoreTracker.GetScorePoint()
            };
        }
        else if (member.ID != 0)
        {
            items[i] = ScoreResultItem{
                member,
                Gx::Randomizer::Randomize<unsigned int>(100, 500),
                Gx::Randomizer::Randomize<unsigned int>(100, 500),
                Gx::Randomizer::Randomize<unsigned int>(0, 10),
                Gx::Randomizer::Randomize<unsigned int>(5, 20),
                Gx::Randomizer::Randomize<unsigned int>(0, 200),
                Gx::Randomizer::Randomize<unsigned int>(1, 20),
                Gx::Randomizer::Randomize<unsigned int>(10000, 20000),
            };
        }
        else
            items[i] = ScoreResultItem{};
    }

    session.SetLatestScoreResults(items);

    CaptureScreen();
    GetDirector().Present<StateResult>();
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

    if (ev.code == sf::Keyboard::Key::Enter)
        m_chatBox->SetEnabled(true);
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
