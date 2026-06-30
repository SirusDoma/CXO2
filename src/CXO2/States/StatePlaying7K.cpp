#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/States/StateWaiting7K.hpp>
#include <CXO2/States/StateResult.hpp>
#include <CXO2/States/StatePlanet.hpp>

#include <CXO2/Core/LifeSystem.hpp>
#include <CXO2/Core/ScoreTracker.hpp>
#include <CXO2/Core/ChartRenderer.hpp>

#include <CXO2/O2.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Config/GameConfig.hpp>

#include <CXO2/Messages/RoomInfo.hpp>
#include <CXO2/Messages/Requests/SubmitScoreRequest.hpp>

#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/UI/Playing/ComboCounter.hpp>
#include <CXO2/UI/Playing/JudgementIndicator.hpp>
#include <CXO2/UI/Playing/PlayMenu.hpp>
#include <CXO2/UI/Playing/Equalizer.hpp>

#include <CXO2/StringTable/Identifiers/Cache.hpp>
#include <CXO2/StringTable/Identifiers/Game.hpp>
#include <CXO2/StringTable/Identifiers/Playing7K.hpp>

#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/Utilities/Randomizer.hpp>
#include <CXO2/Messages/Events/GameCompletedEventData.hpp>
#include <CXO2/Messages/Events/PlayingMemberLeftEventData.hpp>
#include <CXO2/Messages/Events/PlayingMemberScoreSubmissionEventData.hpp>
#include <CXO2/Messages/Events/PlayingMemberStatsUpdateEventData.hpp>
#include <CXO2/States/StateRoom.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    StatePlaying7K::StatePlaying7K(
        Gx::AudioMixer& mixer,
        Gx::ResourceManager& resources,
        PlayingService& service,
        SessionContext& session,
        RoomContext& room,
        GameContext& context,
        GameConfig& config,
        JudgementStrategy& judgementStrategy,
        ScoreTracker& scoreTracker,
        LifeSystem& lifeSystem,
        ItemFactory& items
    ) :
        m_service(service),
        m_session(session),
        m_room(room),
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
        m_renderer.SetName(Resource::Playing7K::IDC_CHART_RENDERER);
        m_renderer.SetRenderCompleteCallback([this]
        {
            if (m_states.size() == 0)
            {
                OnRenderComplete();
                return;
            }

            SubmitScore();
        });
        AddChild(m_renderer);

        m_renderer.Initialize(*m_context.GetChart(), m_context);

        // Setup chat panel
        const auto chatPanel = Instantiate<ChatPanel>(Resource::Playing7K::IDC_CHAT_PANEL);
        m_chatBox = chatPanel->FindChild<Gx::InputField>(Resource::Playing7K::IDC_EDIT_CHAT);
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
        if (const auto instructor = Instantiate<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_INSTRUCTOR))
            instructor->SetVisible(m_context.GetMode() == GameMode::Tutorial);

        if (const auto instruction = Instantiate<Gx::Image>(Resource::Playing7K::IDC_IMAGE_INSTRUCTION))
            instruction->SetVisible(false);

        // Map user states
        if (m_context.GetMode() != GameMode::Tutorial)
        {
            for (std::size_t i = 0; i < RoomContext::MaxCapacity; i++)
            {
                auto& slot = m_room.GetSlot(i);
                if (slot.State != RoomSlotState::Occupied || !slot.Member.has_value())
                    continue;

                m_states[i] = UserState
                {
                    static_cast<std::uint8_t>(i),
                    LifeSystem::DefaultMaxLifePoint,
                    nullptr,
                    true,
                    false
                };
            }
        }

        // Setup avatars + avatars effects'
        const auto avatarList = Instantiate<Gx::List>(Resource::Playing7K::IDC_LIST_AVATAR);
        const auto avaContainers = avatarList->GetChildren();
        for (std::size_t i = 0; i < avaContainers.size(); i++)
        {
            const auto container = dynamic_cast<Gx::UiContainer*>(avaContainers[i]);
            if (!container)
                continue;

            const auto avatar = container->FindChild<Avatar>(Resource::Playing7K::Avatar::IDC_AVATAR);
            if (m_context.GetMode() == GameMode::Tutorial)
            {
                container->SetVisible(i == 4);
                if (!container->IsVisible())
                    continue;

                auto charInfo = m_session.GetCharacterInfo();
                avatar->SetGender(charInfo.Gender);
                for (auto [_, item] : m_items.GetDefaultItems(charInfo.Gender))
                    avatar->SetDefaultItem(std::move(item));

                for (const auto id : charInfo.EquippedItemIDs)
                    avatar->Equip(m_items.Create(id));

                auto slot = RoomSlot
                {
                    /* .Member   = */ charInfo,
                    /* .State    = */ RoomSlotState::Occupied,
                    /* .IsMaster = */ true,
                    /* .Ready    = */ true,
                    /* .Team     = */ RoomTeam::A
                };

                const auto info = avatar->GetAvatarInfo();
                info->SetSlot(slot);

                const auto lifeBar = info->GetLifeBar();
                lifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
                lifeBar->SetValue(m_lifeSystem.GetMaxLifePoint());

                m_self = avatar;
                m_avatars[i] = avatar;
            }
            else
            {
                auto& slot = m_room.GetSlot(i);
                container->SetVisible(slot.State == RoomSlotState::Occupied);

                if (const auto it = m_states.find(i); it != m_states.end())
                    it->second.Avatar = avatar;

                if (!container->IsVisible())
                    continue;

                slot.Ready = slot.IsMaster; // reset ready state early

                avatar->SetGender(slot.Member->Gender);
                for (auto [_, item] : m_items.GetDefaultItems(slot.Member->Gender))
                    avatar->SetDefaultItem(std::move(item));

                for (const auto id : slot.Member->EquippedItemIDs)
                    avatar->Equip(m_items.Create(id));

                auto efc = avatar->FindChild<Gx::UiContainer>(Resource::Playing7K::Avatar::IDC_CONTAINER_EFFECT_JAM);
                auto& effectContainer = efc ? *efc : Create<Gx::UiContainer>();

                effectContainer.SetName(Resource::Playing7K::Avatar::IDC_CONTAINER_EFFECT_JAM);

                auto fx = effectContainer.FindChild<Gx::Animation>(Resource::Playing7K::Avatar::IDC_ANIMATION_EFFECT_JAM);
                if (const auto fxPrefab = Find<Gx::Animation>(Resource::Playing7K::Avatar::IDC_ANIMATION_EFFECT_JAM); !fx && fxPrefab)
                {
                    fx = &Create<Gx::Animation>(*fxPrefab);
                    effectContainer.AddChild(*fx);
                }

                if (fx)
                {
                    fx->SetName(Resource::Playing7K::Avatar::IDC_ANIMATION_EFFECT_JAM);
                    fx->Stop();
                    fx->SetAnimationCallback([=, &effectContainer] (auto& _) {
                        effectContainer.SetVisible(
                            fx->GetState() == Gx::Animation::AnimationState::Playing ||
                            fx->GetState() == Gx::Animation::AnimationState::Initial
                        );
                    });
                }

                auto numEffect = effectContainer.FindChild<Gx::BitmapNumber>(Resource::Playing7K::Avatar::IDC_NUMBER_EFFECT_JAM);
                if (const auto numPrefab = Find<Gx::BitmapNumber>(Resource::Playing7K::Avatar::IDC_NUMBER_EFFECT_JAM); numEffect && numPrefab)
                {
                    numEffect = &Create<Gx::BitmapNumber>(*numPrefab);
                    effectContainer.AddChild(*numEffect);
                }

                if (numEffect)
                {
                    numEffect->SetName(Resource::Playing7K::Avatar::IDC_NUMBER_EFFECT_JAM);
                    numEffect->SetAnimationCallback([=] (auto& _) {
                        numEffect->SetVisible(
                            numEffect->GetAnimationState() == Gx::Animation::AnimationState::Playing ||
                            numEffect->GetAnimationState() == Gx::Animation::AnimationState::Initial
                        );
                    });
                }

                effectContainer.SetVisible(false);
                if (!efc)
                    avatar->AddChild(effectContainer);

                const auto info = avatar->GetAvatarInfo();
                info->SetSlot(slot);

                const auto lifeBar = info->GetLifeBar();
                lifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
                lifeBar->SetValue(m_lifeSystem.GetMaxLifePoint());

                if (m_session.GetCharacterInfo().Name == slot.Member->Name)
                    m_self = avatar;

                m_avatars[i] = avatar;
            }
        }

        // Key down & effects
        const auto keyEffectContainer = Instantiate<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_KEY_EFFECT);
        const auto keyDownContainer = Instantiate<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_KEY_DOWN);
        for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
        {
            const int id = static_cast<int>(channel) - 2;
            if (id < 0 || id > 6)
                continue;

            const auto keyDown = keyDownContainer->FindChild<Gx::Image>(Resource::Playing7K::IDC_IMAGE_KEY_DOWN[id]);
            keyDown->SetVisible(false);

            const auto keyEffect = keyEffectContainer->FindChild<Gx::Image>(Resource::Playing7K::IDC_IMAGE_KEY_EFFECT[id]);
            keyEffect->SetFrame(id);
            keyEffect->SetVisible(false);

            if (m_context.GetMode() == GameMode::Tutorial)
            {
                const auto guideKeyEffect = keyEffectContainer->FindChild<Gx::Image>(Resource::Playing7K::IDC_IMAGE_GUIDE_KEY_EFFECT[id]);
                guideKeyEffect->SetVisible(false);
                m_guideKeyEffects[channel] = guideKeyEffect;
            }

            m_keyDowns[channel]   = keyDown;
            m_keyEffects[channel] = keyEffect;
        }

        // Setup Play Menu
        const auto playMenu = Instantiate<PlayMenu>(Resource::Playing7K::IDC_PLAY_MENU);
        playMenu->SetMetadata(m_context.GetChart()->GetMetadata(), m_context.GetDifficulty());
        playMenu->SetScoreTracker(m_scoreTracker);

        // Setup Score Counter
        const auto scoreNumber = Instantiate<Gx::BitmapNumber>(Resource::Playing7K::IDC_NUMBER_POINT_NUMBER);
        const auto jamGauge = Instantiate<Gx::Gauge>(Resource::Playing7K::IDC_GAUGE_JAM_BAR);
        const auto bufferContainer = Instantiate<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_BUFFER);
        const auto buffers = bufferContainer->GetChildren();
        for (std::size_t i = 0; i < buffers.size(); i++)
        {
            const auto renderable = dynamic_cast<Gx::Renderable*>(buffers[i]);
            renderable->SetVisible(false);
        }

        // Setup Life Bar
        const auto lifeBar = Instantiate<Gx::Gauge>(Resource::Playing7K::IDC_GAUGE_LIFE_BAR);
        if (!O2::InInteropMode(InteropMode::Playing))
            lifeBar->SetSlanted(true);

        lifeBar->SetMaximumValue(m_lifeSystem.GetMaxLifePoint());
        lifeBar->SetValue(0);

          // Setup Jam Combo
        const auto jamContainer = Instantiate<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_NOTE_JAM);
        jamContainer->SetVisible(false);
        const auto jamAnimation = jamContainer->FindChild<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_JAM);
        const auto jamNumber    = jamContainer->FindChild<Gx::BitmapNumber>(Resource::Playing7K::IDC_NUMBER_NOTE_JAM);

        jamContainer->SetVisible(false);
        jamAnimation->Stop();
        jamAnimation->SetAnimationCallback([=] (auto animation) {
            jamContainer->SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
        });

        // Setup Combo Counter
        auto& comboCounter = Create<ComboCounter>(
            Find<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_COMBO),
            Find<Gx::BitmapNumber>(Resource::Playing7K::IDC_NUMBER_NOTE_COMBO)
        );
        comboCounter.SetName(Resource::Playing7K::IDC_CONTAINER_COMBO);
        AddChild(comboCounter);

        // Setup Judgement Indicator
        auto& judgementIndicator = Create<JudgementIndicator>(
            std::unordered_map<Accuracy, Gx::Animation*>
            {
                { Accuracy::Cool, Find<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_COOL) },
                { Accuracy::Good, Find<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_GOOD) },
                { Accuracy::Bad,  Find<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_BAD)  },
                { Accuracy::Miss, Find<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_MISS) },
            }, m_config.UseFx
        );
        judgementIndicator.SetName(Resource::Playing7K::IDC_NOTE_JUDGEMENT_INDICATOR);
        AddChild(judgementIndicator);

        // Setup Long Note effects
        if (const auto longNoteEffectList = Find<Gx::List>(Resource::Playing7K::IDC_LIST_LONG_NOTE_EFFECT); longNoteEffectList)
        {
            for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
            {
                const int id = static_cast<int>(channel) - 2;
                if (id < 0 || id > 6)
                    continue;

                if (const auto longNoteEffect = longNoteEffectList->FindChild<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_LONG_NOTE_EFFECT[id]); longNoteEffect)
                {
                    longNoteEffect->SetVisible(false);
                    m_longNoteEffects[channel] = longNoteEffect;

                    AddChild(*longNoteEffect);
                }
            }
        }

        // Setup Note Clicks
        if (const auto noteClickList = Find<Gx::List>(Resource::Playing7K::IDC_LIST_NOTE_CLICK); noteClickList)
        {
            for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
            {
                const int id = static_cast<int>(channel) - 2;
                if (id < 0 || id > 6)
                    continue;

                if (const auto noteClick = noteClickList->FindChild<Gx::Animation>(Resource::Playing7K::IDC_ANIMATION_NOTE_CLICK[id]); noteClick)
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
            if (const auto equalizer = playMenu->FindChild<Equalizer>(Resource::Game::Menu::IDC_EQUALIZER))
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
                    guideKeyEffect->second->SetFrame(fmt::format("{}B", static_cast<int>(channel) - 2));
                else
                    guideKeyEffect->second->SetFrame(fmt::format("{}A", static_cast<int>(channel) - 2));
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
            if (m_scoreTracker.IsEnabled())
            {
                const auto lastLife = m_lifeSystem.GetCurrentLifePoint();
                m_lifeSystem.Update(acc, count);

                const auto currentLife = m_lifeSystem.GetCurrentLifePoint();
                if (lastLife != currentLife)
                {
                    m_service.UpdateLife(
                        currentLife,
                        [] {},
                        [=] (const auto& ex)
                        {
                            Invoke([=]
                            {
                                ShowDialog(std::string(ex.what()), DialogStyle::Information, false);
                                GetDirector().Dismiss<StatePlanet>();
                            });
                        }
                    );
                }

                m_self->GetAvatarInfo()->GetLifeBar()->SetValue(m_lifeSystem.GetCurrentLifePoint());
                lifeBar->SetValue(m_lifeSystem.GetCurrentLifePoint());

                if (m_lifeSystem.GetCurrentLifePoint() == 0)
                {
                    m_scoreTracker.SetEnabled(false);
                    m_self->Die();

                    SubmitScore();
                    if (m_context.GetDifficulty() != Difficulty::EX && m_states.size() == 0)
                    {
                        Run<Gx::Delay>(sf::milliseconds(2000), [this]
                        {
                            OnRenderComplete();
                        });

                        return;
                    }
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

            PlayAvatarJamCombo(m_self, jamCombo);

            m_service.UpdateJam(
                jamCombo,
                [] {},
                [=] (const auto& ex)
                {
                    Invoke([=]
                    {
                        ShowDialog(std::string(ex.what()), DialogStyle::Information, false);
                        GetDirector().Dismiss<StatePlanet>();
                    });
                }
            );
        });

        // Exit button
        const auto exitButton = Instantiate<Gx::Button>(Resource::Playing7K::IDC_BUTTON_EXIT);
        exitButton->SetClickCallback([this] (const auto& sender, const auto& ev)
        {
            m_service.ExitPlaying([=]
            {
                Invoke([=]
                {
                    if (m_context.GetMode() != GameMode::Single)
                    {
                        GetDirector().Dismiss<StateRoom>();
                    }
                    else
                        GetDirector().Dismiss();
                });
            });

        });

        for (auto [_, avatar] : m_avatars)
            avatar->GetAvatarInfo()->GetLifeBar()->SetValue(avatar->GetAvatarInfo()->GetLifeBar()->GetMaximumValue());

        // Start initial lifebar fill-up animation
        Run<Gx::Scheduler>
        (
            sf::seconds(2.f),
            sf::seconds(1.f / 60.f),
            [this, lifeBar] (const auto& task, auto delta)
            {
                lifeBar->SetValue(lifeBar->GetValue() + m_lifeSystem.GetMaxLifePoint() / (2.f / (delta / 1000.f)));
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
            GetDirector().Dismiss();
            return;
        }

        if (m_states.size() == 0)
        {
            auto items = std::array<ScoreEntry, 8>();
            for (std::size_t i = 0; i < items.size(); i++)
            {
                auto& slot = m_room.GetSlot(i);
                if (slot.Member.has_value() && slot.Member->Name == m_session.GetCharacterInfo().Name)
                {
                    items[i] = ScoreEntry
                    {
                        /* .ID          = */ static_cast<std::uint8_t>(i),
                        /* .Active      = */ true,
                        /* .Cool        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Cool)),
                        /* .Good        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Good)),
                        /* .Bad         = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Bad)),
                        /* .Miss        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Miss)),
                        /* .MaxCombo    = */ static_cast<std::uint16_t>(m_scoreTracker.GetMaxCombo()),
                        /* .MaxJamCombo = */ static_cast<std::uint16_t>(m_scoreTracker.GetMaxJamCombo()),
                        /* .Score       = */ static_cast<std::uint32_t>(m_scoreTracker.GetScorePoint()),
                        /* .Reward      = */ 0,
                        /* .Level       = */ m_session.GetCharacterInfo().Level,
                        /* .Experience  = */ m_session.GetCharacterInfo().Experience,
                        /* .Winning     = */ true,
                        /* .Reserved    = */ {},

                    };
                }
                else
                    items[i] = ScoreEntry{};
            }

            m_context.SetScoreEntries(items);
        }

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

    void StatePlaying7K::OnMemberStatsUpdate(const PlayingMemberStatsUpdateEventData& ev)
    {
        if (ev.Type == UpdateStatsType::Life)
        {
            const auto it = m_states.find(ev.ID);
            if (it == m_states.end() || !it->second.Valid || it->second.Avatar == m_self)
                return;

            it->second.Life = ev.Value;
            if (const auto avatar = it->second.Avatar)
            {
                avatar->GetAvatarInfo()->GetLifeBar()->SetValue(ev.Value);
                if (ev.Value == 0)
                    avatar->Die();
            }

            if (ev.Value == 0)
                it->second.Completed = true;
        }
        else
        {
            const auto it = m_states.find(ev.ID);
            if (it == m_states.end() || !it->second.Valid || it->second.Avatar == m_self)
                return;

            if (const auto avatar = it->second.Avatar)
            {
                if (ev.Value != 0)
                    PlayAvatarJamCombo(avatar, ev.Value);
            }
        }
    }

    void StatePlaying7K::OnMemberScoreSubmitted(const PlayingMemberScoreSubmissionEventData& ev)
    {
        const auto it = m_states.find(ev.ID);
        if (it == m_states.end() || !it->second.Valid)
            return;

        it->second.Completed = true;
    }

    void StatePlaying7K::OnMemberLeft(const PlayingMemberLeftEventData& ev)
    {
        if (const auto it = m_states.find(ev.ID); it != m_states.end())
        {
            it->second.Completed = true;
            it->second.Valid = false;

            if (it->second.Avatar)
                it->second.Avatar->SetVisible(false);

            if (ev.ID < 0 || ev.ID >= RoomContext::MaxCapacity)
                return;

            auto& slot = m_room.GetSlot(ev.ID);
            auto name = slot.Member->Name;

            slot.State    = RoomSlotState::Unoccupied;
            slot.IsMaster = false;
            slot.Ready    = false;
            slot.Member   = std::nullopt;
        }
    }

    void StatePlaying7K::OnGameCompleted(const GameCompletedEventData& ev)
    {
        auto entries = std::array<ScoreEntry, RoomContext::MaxCapacity>();

        const auto& container = ev.Entries.GetContainer();
        for (std::size_t i = 0; i < entries.size(); i++)
        {
            if (i < container.size() && container[i].Active)
                entries[i] = container[i];
        }

        m_context.SetScoreEntries(entries);
        OnRenderComplete();
    }

    void StatePlaying7K::Update(const sf::Time& delta)
    {
        State::Update(delta);

        if (m_context.GetMode() == GameMode::Tutorial)
        {
            const auto keyEffectContainer = Instantiate<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_KEY_EFFECT);
            const auto frontBuffers = m_renderer.GetFrontBuffers();
            for (auto [channel, _] : m_config.KeyBindings.at(KeyMode::Seven))
            {
                const int id = static_cast<int>(channel) - 2;
                if (id < 0|| id > 6)
                    continue;

                const auto guideKeyEffect = keyEffectContainer->FindChild<Gx::Image>(Resource::Playing7K::IDC_IMAGE_GUIDE_KEY_EFFECT[id]);
                const auto frameName      = fmt::format("{}A", id);
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
                        m_guideKeyEffectDeltas[channel] += delta.asMilliseconds();
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

            const auto instruction = Instantiate<Gx::Image>(Resource::Playing7K::IDC_IMAGE_INSTRUCTION);
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

    void StatePlaying7K::PlayAvatarJamCombo(const Avatar* avatar, const std::uint16_t jams)
    {
        const auto effectContainer = avatar->FindChild<Gx::UiContainer>(Resource::Playing7K::Avatar::IDC_CONTAINER_EFFECT_JAM);
        if (!effectContainer)
            return;

        for (const auto child : effectContainer->GetChildren())
        {
            if (const auto number = dynamic_cast<Gx::BitmapNumber*>(child); number)
            {
                number->SetValue(jams);
                number->Reset();
            }

            if (const auto animation = dynamic_cast<Gx::Animation*>(child); animation)
                animation->Reset();
        }
    }

    void StatePlaying7K::SubmitScore()
    {
        m_service.SubmitScore(SubmitScoreRequest
        {
            /* .Cool        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Cool)),
            /* .Good        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Good)),
            /* .Bad         = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Bad)),
            /* .Miss        = */ static_cast<std::uint16_t>(m_scoreTracker.GetPoint(Accuracy::Miss)),
            /* .MaxCombo    = */ static_cast<std::uint16_t>(m_scoreTracker.GetMaxCombo()),
            /* .JamCombo    = */ static_cast<std::uint16_t>(m_scoreTracker.GetJamCombo()),
            /* .MaxJamCombo = */ static_cast<std::uint16_t>(m_scoreTracker.GetMaxJamCombo()),
            /* .Score       = */ static_cast<std::uint32_t>(m_scoreTracker.GetScorePoint()),
            /* .Life        = */ static_cast<std::uint8_t>(m_lifeSystem.GetCurrentLifePoint() / m_lifeSystem.GetMaxLifePoint() * 100),
        },
        [=]{},
        [=] (const auto& ex)
        {
            Invoke([=]
            {
                ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
                {
                    GetDirector().Dismiss<StatePlanet>();
                });
            });
        });
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

    Gx::RenderStates StatePlaying7K::Render(Gx::RenderSurface& surface, const Gx::RenderStates states) const
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

            Update(sf::Time::Zero);
            surface.Render(*this, Gx::RenderStates::Default);
        }
        target.display();

        auto texture = std::make_unique<sf::Texture>(target.getTexture());
        resources->Store<sf::Texture>(Resource::Cache::IDC_TEXTURE_STATE_PLAYING, std::move(texture), Gx::CacheMode::Update);
    }
}
