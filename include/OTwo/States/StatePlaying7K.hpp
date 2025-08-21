#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Core/Chart.hpp>
#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/InputField.hpp>

#include <unordered_map>

struct PlayingMemberStatsUpdateEventData;
struct PlayingMemberScoreSubmissionEventData;
struct PlayingMemberLeftEventData;
struct GameCompletedEventData;

class PlayingService;
class SessionContext;
class RoomContext;
class GameContext;
class ItemFactory;
class Avatar;
class StatePlaying7K : public State
{
public:
    StatePlaying7K(
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
    );

    void Initialize() override;

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    void OnMemberStatsUpdate(const PlayingMemberStatsUpdateEventData& ev);
    void OnMemberScoreSubmitted(const PlayingMemberScoreSubmissionEventData& ev);
    void OnMemberLeft(const PlayingMemberLeftEventData& ev);
    void OnGameCompleted(const GameCompletedEventData& ev);

    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
    void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

private:
    struct UserState
    {
        std::uint8_t  ID{};
        std::uint16_t Life{};
        ::Avatar*     Avatar{nullptr};
        bool          Valid{};
        bool          Completed{};
    };

    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
    void Update(const double delta) override;

    void PlayAvatarJamCombo(const Avatar* avatar, std::uint16_t jams);

    void SubmitScore();

    void OnRenderComplete();
    void CaptureScreen();

    using ImageMap = std::unordered_map<Chart::Channel, Gx::Image*>;
    using InputStateMap = std::unordered_map<Chart::Channel, bool>;
    using AnimationMap = std::unordered_map<Chart::Channel, Gx::Animation*>;
    using AvatarMap = std::unordered_map<unsigned int, Avatar*>;

    PlayingService& m_service;
    SessionContext& m_session;
    RoomContext& m_room;
    GameContext& m_context;
    GameConfig& m_config;
    ScoreTracker& m_scoreTracker;
    LifeSystem& m_lifeSystem;
    ItemFactory& m_items;

    ChartRenderer m_renderer;
    AvatarMap m_avatars;
    AnimationMap m_noteClicks;
    AnimationMap m_longNoteEffects;
    Avatar* m_self;

    Gx::InputField* m_chatBox;
    ImageMap m_keyDowns, m_keyEffects, m_guideKeyEffects;
    InputStateMap m_inputStates;
    InputStateMap m_guideKeyEffectStates;
    std::unordered_map<Chart::Channel, float> m_guideKeyEffectDeltas;

    std::unordered_map<std::uint8_t, UserState> m_states{};

    unsigned int m_viewport;
};
