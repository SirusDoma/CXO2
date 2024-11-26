#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Core/LifeSystem.hpp>
#include <OTwo/Core/ScoreTracker.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>
#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/UI/Playing/JudgementIndicator.hpp>
#include <OTwo/UI/Playing/PlayMenu.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/Utilities/Randomizer.hpp>

StatePlaying7K::StatePlaying7K(
    Gx::AudioMixer& mixer,
    SessionContext& session,
    GameContext& context,
    GameConfig& config,
    JudgementStrategy& judgementStrategy,
    ScoreTracker& scoreTracker,
    LifeSystem& lifeSystem,
    ItemFactory& items
) :
    m_session(session),
    m_context(context),
    m_config(config),
    m_scoreTracker(scoreTracker),
    m_lifeSystem(lifeSystem),
    m_items(items),
    m_renderer(
        judgementStrategy,
        lifeSystem,
        scoreTracker,
        mixer,
        GetResources(),
        ChannelSet{
           Chart::Channel::Note1,
           Chart::Channel::Note2,
           Chart::Channel::Note3,
           Chart::Channel::Note4,
           Chart::Channel::Note5,
           Chart::Channel::Note6,
           Chart::Channel::Note7
       }
    ),
   m_self(),
   m_chatBox(),
   m_viewport()
{
}

void StatePlaying7K::Initialize()
{
    State::Initialize();

    // Setup providers
    m_context.SetViewport(GetViewport());
    m_scoreTracker.Initialize(m_context.GetDifficulty());
    m_lifeSystem.Initialize(m_context.GetDifficulty());

    // Add chart renderer
    m_renderer.SetName("IDC_CHART_RENDERER");
    m_renderer.SetRenderCompleteCallback([this] { OnRenderComplete(); });
    AddChild(m_renderer);

    m_renderer.Initialize(*m_context.GetChart(), m_context);

    // Setup chat panel
    const auto chatPanel = Instantiate<ChatPanel>("IDC_CHAT_PANEL");
    m_chatBox = chatPanel->FindChild<Gx::InputField>("IDC_EDIT_CHAT");
    if (m_context.GetMode() != GameMode::Tutorial)
    {
        chatPanel->SetMaximumTextLength(50);
        m_chatBox->SetPermanentFocusEnabled(true);
        m_chatBox->SetEnabled(false);
    }
    else
    {
        chatPanel->SetEnabled(false);
        chatPanel->SetVisible(false);
    }

    // Setup Guide
    if (const auto instructor = Instantiate<Gx::Animation>("IDC_ANIMATION_INSTRUCTOR"))
        instructor->SetVisible(m_context.GetMode() == GameMode::Tutorial);

    if (const auto instruction = Instantiate<Gx::Image>("IDC_IMAGE_INSTRUCTION"))
        instruction->SetVisible(false);

    // Setup avatars + avatars effects'
    const auto avatarList = Instantiate<Gx::List>("IDC_LIST_AVATAR");
    const auto avaContainers = avatarList->GetChildren();
    for (std::size_t i = 0; i < avaContainers.size(); i++)
    {
        const auto container = dynamic_cast<Gx::UiContainer*>(avaContainers[i]);
        if (!container)
            continue;

        const auto avatar = container->FindChild<Avatar>("IDC_AVATAR");
        if (m_context.GetMode() == GameMode::Tutorial)
        {
            container->SetVisible(i == 4);
            if (!container->IsVisible())
                continue;

            auto player = m_session.GetCurrentPlayer();
            for (const auto id : player.EquippedItemIDs)
                avatar->Equip(m_items.Create(id));

            auto member  = RoomMember{player};
            member.Team  = RoomTeam::A;
            member.Color = sf::Color(255, 0, 21);

            const auto info = avatar->GetAvatarInfo();
            info->SetMember(member);

            const auto playerLifeBar = info->GetLifeBar();
            playerLifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
            playerLifeBar->SetValue(m_lifeSystem.GetMaxLifePoint());

            if (m_session.GetCurrentPlayer().ID == member.ID)
                m_self = avatar;

            m_avatars[i] = avatar;
        }
        else
        {
            auto& room = m_session.GetCurrentRoom();
            if (i >= room.MaxCapacity)
                break;

            auto member = room.Members[i];
            container->SetVisible(member.ID != 0);

            if (!container->IsVisible())
                continue;

            for (const auto id : member.EquippedItemIDs)
                avatar->Equip(m_items.Create(id));

            auto& effectContainer = Create<Gx::UiContainer>();
            effectContainer.SetName("IDC_CONTAINER_EFFECT_JAM");

            if (const auto fxPrefab = FindResource<Gx::Animation>("IDC_ANIMATION_EFFECT_JAM"); fxPrefab)
            {
                auto& fx = Create<Gx::Animation>(*fxPrefab);
                fx.SetName("IDC_ANIMATION_EFFECT_JAM");
                fx.Stop();
                fx.SetAnimationCallback([&] (auto& _) {
                    effectContainer.SetVisible(
                        fx.GetState() == Gx::Animation::AnimationState::Playing ||
                        fx.GetState() == Gx::Animation::AnimationState::Initial
                    );
                });

                effectContainer.AddChild(fx);
            }

            if (const auto numPrefab = FindResource<Gx::BitmapNumber>("IDC_NUMBER_EFFECT_JAM"); numPrefab)
            {
                auto& numEffect = Create<Gx::BitmapNumber>(*numPrefab);
                numEffect.SetName("IDC_NUMBER_EFFECT_JAM");
                numEffect.SetAnimationCallback([&] (auto& _) {
                    numEffect.SetVisible(
                        numEffect.GetAnimationState() == Gx::Animation::AnimationState::Playing ||
                        numEffect.GetAnimationState() == Gx::Animation::AnimationState::Initial
                    );
                });
                effectContainer.AddChild(numEffect);
            }

            effectContainer.SetVisible(false);
            avatar->AddChild(effectContainer);

            const auto info = avatar->GetAvatarInfo();
            info->SetMember(member);

            const auto playerLifeBar = info->GetLifeBar();
            playerLifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
            playerLifeBar->SetValue(m_lifeSystem.GetMaxLifePoint());

            if (m_session.GetCurrentPlayer().ID == member.ID)
                m_self = avatar;

            m_avatars[i] = avatar;
        }
    }

    // Key down & effects
    const auto keyEffectContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT");
    const auto keyDownContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_DOWN");
    for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
    {
        const int id = static_cast<int>(channel) - 1;
        if (id < 1 || id > 7)
            continue;

        const auto keyDown = keyDownContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_DOWN" + std::to_string(id));
        keyDown->SetVisible(false);

        const auto keyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_KEY_EFFECT" + std::to_string(id));
        keyEffect->SetFrame(id - 1);
        keyEffect->SetVisible(false);

        if (m_context.GetMode() == GameMode::Tutorial)
        {
            const auto guideKeyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_GUIDE_KEY_EFFECT" + std::to_string(id));
            guideKeyEffect->SetVisible(false);
            m_guideKeyEffects[channel] = guideKeyEffect;
        }

        m_keyDowns[channel]   = keyDown;
        m_keyEffects[channel] = keyEffect;
    }

    // Setup Play Menu
    const auto playMenu = Instantiate<PlayMenu>("IDC_PLAY_MENU");
    playMenu->SetMetadata(m_context.GetChart()->GetMetadata().ToChartMetadataView(m_context.GetDifficulty()), m_context.GetDifficulty());
    playMenu->SetScoreTracker(m_scoreTracker);

    // Setup Score Counter
    const auto scoreNumber = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_POINT_NUMBER");
    const auto jamGauge = Instantiate<Gx::Gauge>("IDC_GAUGE_JAM_BAR");
    const auto bufferContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_BUFFER");
    const auto buffers = bufferContainer->GetChildren();
    for (std::size_t i = 0; i < buffers.size(); i++)
    {
        const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
        renderable->SetVisible(false);
    }

    // Setup Life Bar
    const auto lifeBar = Instantiate<Gx::Gauge>("IDC_GAUGE_LIFE_BAR");
    lifeBar->SetSlanted(true);
    lifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
    lifeBar->SetValue(0);

      // Setup Jam Combo
    const auto jamContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_NOTE_JAM");
    jamContainer->SetVisible(false);
    const auto jamAnimation = jamContainer->FindChild<Gx::Animation>("IDC_ANIMATION_NOTE_JAM");
    const auto jamNumber    = jamContainer->FindChild<Gx::BitmapNumber>("IDC_NUMBER_NOTE_JAM");

    jamContainer->SetVisible(false);
    jamAnimation->Stop();
    jamAnimation->SetAnimationCallback([=] (auto animation) {
        jamContainer->SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
    });

    // Setup Combo Counter
    auto& comboCounter = Create<ComboCounter>(
        FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_COMBO"),
        FindResource<Gx::BitmapNumber>("IDC_NUMBER_NOTE_COMBO")
    );
    comboCounter.SetName("IDC_CONTAINER_COMBO");
    AddChild(comboCounter);

    // Setup Judgement Indicator
    auto& judgementIndicator = Create<JudgementIndicator>(
        std::unordered_map<Accuracy, Gx::Animation*>
        {
            { Accuracy::Cool, FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_COOL") },
            { Accuracy::Good, FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_GOOD") },
            { Accuracy::Bad,  FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_BAD")  },
            { Accuracy::Miss, FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_MISS") },
        }, m_config.UseFx
    );
    judgementIndicator.SetName("IDC_NOTE_JUDGEMENT_INDICATOR");
    AddChild(judgementIndicator);

    // Setup Long Note effects
    if (const auto longNoteEffectList = FindResource<Gx::List>("IDC_LIST_LONG_NOTE_EFFECT"); longNoteEffectList)
    {
        for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
        {
            const int id = static_cast<int>(channel) - 1;
            if (id < 1 || id > 7)
                continue;

            if (const auto longNoteEffect = longNoteEffectList->FindChild<Gx::Animation>("IDC_ANIMATION_LONG_NOTE_EFFECT" + std::to_string(id)); longNoteEffect)
            {
                longNoteEffect->SetVisible(false);
                m_longNoteEffects[channel] = longNoteEffect;

                AddChild(*longNoteEffect);
            }
        }
    }

    // Setup Note Clicks
    if (const auto noteClickList = FindResource<Gx::List>("IDC_LIST_NOTE_CLICK"); noteClickList)
    {
        for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
        {
            const int id = static_cast<int>(channel) - 1;
            if (id < 1 || id > 7)
                continue;

            if (const auto noteClick = noteClickList->FindChild<Gx::Animation>("IDC_ANIMATION_NOTE_CLICK" + std::to_string(id)); noteClick)
            {
                noteClick->SetVisible(false);
                noteClick->Stop();
                noteClick->SetAnimationCallback([] (auto& animation) {
                    animation.SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
                });

                m_noteClicks[channel] = noteClick;
                AddChild(*noteClick);
            }
        }
    }

    // Setup equalizer
    if (m_config.UseEqualizer)
    {
        if (const auto equalizer = playMenu->FindChild<Equalizer>("IDC_EQUALIZER"))
            m_renderer.SetEqualizer(*equalizer);
    }

    // Setup Key Effects
    m_renderer.SetInputCallback([=] (auto channel, bool state)
    {
        m_inputStates[channel] = state;
        if (const auto keyEffect = m_keyEffects.find(channel); keyEffect != m_keyEffects.end())
            keyEffect->second->SetVisible(state);

        if (const auto guideKeyEffect = m_guideKeyEffects.find(channel); guideKeyEffect != m_guideKeyEffects.end())
        {
            guideKeyEffect->second->SetVisible(state);
            if (state)
                guideKeyEffect->second->SetFrame(std::to_string(static_cast<int>(channel) - 2) + "B");
            else
                guideKeyEffect->second->SetFrame(std::to_string(static_cast<int>(channel) - 2) + "A");
        }

        if (const auto keyDown = m_keyDowns.find(channel); keyDown != m_keyDowns.end())
            keyDown->second->SetVisible(state);
    });

    // Setup Score changes
    m_scoreTracker.AddIncrementListener([=, &comboCounter, &judgementIndicator] (auto& ev, auto acc, auto count)
    {
        if (!m_scoreTracker.IsEnabled() && m_context.GetDifficulty() != Difficulty::EX)
            return;

        // Life System
        m_lifeSystem.Update(acc, count);
        m_self->GetAvatarInfo()->GetLifeBar()->SetValue(m_lifeSystem.GetCurrentLifePoint());
        lifeBar->SetValue(m_lifeSystem.GetCurrentLifePoint());

        if (m_scoreTracker.IsEnabled() && m_lifeSystem.GetCurrentLifePoint() == 0)
        {
            m_scoreTracker.SetEnabled(false);
            m_self->Die();

            if (m_context.GetDifficulty() != Difficulty::EX)
            {
                Run<Gx::Delay>(sf::milliseconds(2000), [this]
                {
                    OnRenderComplete();
                });

                return;
            }
        }

        // Note Click and Long Note Effects
        m_noteClicks[ev.Channel]->Reset();
        m_longNoteEffects[ev.Channel]->SetVisible(false);
        if (acc == Accuracy::Bad || acc == Accuracy::Miss)
        {
            m_noteClicks[ev.Channel]->Stop();
            if (m_config.UseFx)
                m_longNoteEffects[ev.Channel]->SetVisible(false);
        }
        else if (ev.Type == Chart::NoteType::Hold && m_config.UseFx)
        {
            m_longNoteEffects[ev.Channel]->Reset();
            m_longNoteEffects[ev.Channel]->SetVisible(true);
        }

        // Combo
        comboCounter.SetCombo(m_scoreTracker.GetCombo());
        if (m_lifeSystem.GetCurrentLifePoint() > 0 || acc != Accuracy::Miss)
            judgementIndicator.Play(acc);
        else
            judgementIndicator.Play(Accuracy::None);

        // Score and Jam Combo
        scoreNumber->SetValue(m_scoreTracker.GetScorePoint());
        jamGauge->SetValue(m_scoreTracker.GetJamProgress());

        // Buffer
        for (std::size_t i = 0; i < buffers.size(); i++)
        {
            const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
            renderable->SetVisible(i < m_scoreTracker.GetBufferCount());
        }
    });

    // Setup jam combo effect
    m_scoreTracker.AddJamComboListener([=] (auto& ev, auto acc, auto jamCombo)
    {
        jamNumber->SetValue(jamCombo);
        jamAnimation->Reset();
        jamContainer->SetVisible(true);

        const auto effectContainer = m_self->FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_JAM");
        if (!effectContainer)
            return;

        for (const auto child : effectContainer->GetChildren())
        {
            if (const auto number = dynamic_cast<Gx::BitmapNumber*>(child); number)
            {
                number->SetValue(jamCombo);
                number->Reset();
            }

            if (const auto animation = dynamic_cast<Gx::Animation*>(child); animation)
                animation->Reset();
        }
    });

    // Exit button
    const auto exitButton = Instantiate<Gx::Button>("IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([this] (const auto& sender, const auto& ev)
    {
        if (m_context.GetMode() == GameMode::Tutorial)
            GetDirector().Present<StateRoom>();
        else
            GetDirector().Present<StateWaiting7K>();
    });

    // Start initial lifebar fill-up animation
    Run<Gx::Scheduler>
    (
        sf::seconds(2.f),
        sf::seconds(1.f / 60.f),
        [this, lifeBar] (const auto& task, auto delta)
        {
            lifeBar->SetValue(lifeBar->GetValue() + m_lifeSystem.GetMaxLifePoint() / (2.f / (delta / 1000.f)));

            // TODO: There's no need to animate avatar life bar?
            for (auto [_, avatar] : m_avatars)
                avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

            if (task.GetState() == Gx::TaskState::Completed)
            {
                lifeBar->SetValue(m_lifeSystem.GetMaxLifePoint());
                for (auto [_, avatar] : m_avatars)
                    avatar->GetAvatarInfo()->GetLifeBar()->SetValue(lifeBar->GetValue());

                m_renderer.StartRender();
            }
        }
    );
}

void StatePlaying7K::OnRenderComplete()
{
    if (m_context.GetMode() == GameMode::Tutorial)
    {
        QueueEvent([this]
        {
            GetDirector().Present<StateRoom>();
        });

        return;
    }

    auto items = std::array<ScoreResultItem, 8>();
    for (std::size_t i = 0; i < items.size(); i++)
    {
        auto& member = m_session.GetCurrentRoom().Members[i];
        if (member.ID == m_session.GetCurrentPlayer().ID)
        {
            items[i] = ScoreResultItem{
                member,
                m_scoreTracker.GetPoint(Accuracy::Cool),
                m_scoreTracker.GetPoint(Accuracy::Good),
                m_scoreTracker.GetPoint(Accuracy::Bad),
                m_scoreTracker.GetPoint(Accuracy::Miss),
                m_scoreTracker.GetMaxCombo(),
                m_scoreTracker.GetMaxJamCombo(),
                m_scoreTracker.GetScorePoint()
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

    CaptureScreen();

    QueueEvent([this, items]
    {
        m_session.SetLatestScoreResults(items);
        GetDirector().Present<StateResult>();
    });
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

    if (m_context.GetMode() == GameMode::Tutorial)
    {
        const auto keyEffectContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_KEY_EFFECT");
        const auto frontBuffers = m_renderer.GetFrontBuffers();
        for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
        {
            const int id = static_cast<int>(channel) - 2;
            if (id < 0|| id > 6)
                continue;

            const auto guideKeyEffect = keyEffectContainer->FindChild<Gx::Image>("IDC_IMAGE_GUIDE_KEY_EFFECT" + std::to_string(id + 1));
            const auto frameName      = std::to_string(id) + "A";
            if (auto buffer = frontBuffers.find(channel); buffer != frontBuffers.end() && buffer->second)
            {
                const double position = buffer->second->Event->Position - m_renderer.GetRenderPosition();
                if (position >= 1.5f || buffer->second->IsRegistered())
                {
                    m_guideKeyEffectDeltas[channel] = 0;
                    m_guideKeyEffectStates[channel] = false;
                }
                else
                {
                    m_guideKeyEffectDeltas[channel] += delta;
                    if (m_guideKeyEffectDeltas[channel] >= 500)
                    {
                        m_guideKeyEffectDeltas[channel] = 0;
                        m_guideKeyEffectStates[channel] = !m_guideKeyEffectStates[channel];
                    }
                }

                if (guideKeyEffect->IsVisible() != m_guideKeyEffectStates[channel] && !m_inputStates[channel])
                {
                    guideKeyEffect->SetFrame(frameName);
                    guideKeyEffect->SetVisible(m_guideKeyEffectStates[channel]);
                }
            }
        }

        const auto instruction = Instantiate<Gx::Image>("IDC_IMAGE_INSTRUCTION");
        const double position  = m_renderer.GetRenderPosition();
        if (instruction && position >= 9.f)
        {
            instruction->SetVisible(true);
            if (position >= 111.5f)
                instruction->SetFrame(12);
            else if (position >= 104.5f)
                instruction->SetFrame(11);
            else if (position >= 97.5f)
                instruction->SetFrame(10);
            else if (position >= 90.f)
                instruction->SetFrame(9);
            else if (position >= 77.f)
                instruction->SetFrame(8);
            else if (position >= 70.5f)
                instruction->SetFrame(7);
            else if (position >= 61.f)
                instruction->SetFrame(6);
            else if (position >= 53.f)
                instruction->SetFrame(5);
            else if (position >= 45.5f)
                instruction->SetFrame(4);
            else if (position >= 35.f)
                instruction->SetFrame(3);
            else if (position >= 31.f)
                instruction->SetFrame(2);
            else if (position >= 19.5f)
                instruction->SetFrame(1);
            else if (position > 9.f)
                instruction->SetFrame(0);
        }
    }
}

void StatePlaying7K::OnKeyPressed(const sf::Event::KeyPressed& ev)
{
    Inputable::OnKeyPressed(ev);
}

void StatePlaying7K::OnKeyReleased(const sf::Event::KeyReleased& ev)
{
    Inputable::OnKeyReleased(ev);

    if (ev.code == sf::Keyboard::Key::F5)
    {
        if (m_config.NoteShapeType == NoteShape::Square)
            m_config.NoteShapeType = NoteShape::Circle;
        else
            m_config.NoteShapeType = NoteShape::Square;
    }

    if (ev.code == sf::Keyboard::Key::F6)
    {
        if (m_config.NoteGuideLength == 3)
            m_config.NoteGuideLength = 0;
        else
            m_config.NoteGuideLength++;
    }

    if (ev.code == sf::Keyboard::Key::Enter)
        m_chatBox->SetEnabled(true);

    if (ev.code == sf::Keyboard::Key::Escape)
    {
        m_chatBox->SetString(std::string());
        m_chatBox->SetEnabled(false);
    }
}

Gx::RenderStates StatePlaying7K::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    return State::Render(surface, states);
}

void StatePlaying7K::CaptureScreen()
{
    const auto resources = &GetResources(ResourceScope::Shared);
    const sf::RenderTarget& window = GetApplication();

    auto target = sf::RenderTexture(window.getSize());
    target.clear(GetApplication().GetClearColor());
    {
        auto surface = Gx::RenderSurfaceAdaptor(target);

        Update(0);
        surface.Render(*this, Gx::RenderStates::Default);
    }
    target.display();

    auto texture = std::make_unique<sf::Texture>(target.getTexture());
    resources->Store<sf::Texture>("IDC_TEXTURE_STATE_PLAYING", std::move(texture), Gx::CacheMode::Update);
}
