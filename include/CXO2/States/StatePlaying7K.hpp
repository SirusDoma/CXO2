#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Models/Room.hpp>
#include <CXO2/Core/Chart.hpp>
#include <CXO2/Core/ChartRenderer.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/InputField.hpp>

#include <unordered_map>
#include <vector>

namespace Cx
{
    class SessionContext;
    class RoomContext;
    class ItemFactory;
    class Avatar;
    class ComboCounter;
    class JudgementIndicator;
    class StatePlaying7K : public State
    {
    public:
        StatePlaying7K(
            Gx::AudioMixer& mixer,
            Gx::ResourceManager& resources,
            PlayingService& service,
            SessionContext& session,
            RoomContext& room,
            GameConfig& config,
            JudgementStrategy& judgementStrategy,
            LifeSystem& lifeSystem,
            ItemFactory& items
        );

        void Initialize() override;
        void Initialize(GameContext game);

        unsigned int GetViewport() const;
        void SetViewport(unsigned int viewport);

        void OnMemberStatsUpdate(const MessageEnvelope<PlayingMemberStatsUpdateEventData>& ev);
        void OnMemberScoreSubmitted(const MessageEnvelope<PlayingMemberScoreSubmissionEventData>& ev);
        void OnMemberLeft(const MessageEnvelope<PlayingMemberLeftEventData>& ev);
        void OnGameCompleted(const MessageEnvelope<GameCompletedEventData>& ev);

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

    private:
        struct UserState
        {
            std::uint8_t  ID{};
            std::uint16_t Life{};
            Cx::Avatar*     Avatar{nullptr};
            bool          Valid{};
            bool          Completed{};
        };

        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        void Update(const sf::Time& delta) override;

        void PlayAvatarJamCombo(const Avatar* avatar, std::uint16_t jams);
        void EquipAvatar(Avatar* avatar, Gender gender, const EquipmentSet& equippedItemIDs);
        void SetupAvatarInfo(Avatar* avatar, const Room::Slot& slot);

        void SubmitScore();
        void SetScores(const std::array<GameCompletedEventData::ScoreEntry, 8>& entries);

        void OnRenderComplete();
        void CaptureScreen();

        void OnUpdateGameStatsResponded(const MessageEnvelope<UpdateGameStatsRequest>& ev);
        void OnSubmitScoreResponded(const MessageEnvelope<SubmitScoreRequest>& ev);
        void OnExitPlayingResponded(const MessageEnvelope<ExitPlayingRequest>& ev);

        void OnChartRenderCompleted();
        void OnChartInput(Chart::Channel channel, bool state);

        void OnScoreIncremented(const Chart::NoteEvent& ev, Accuracy acc, unsigned long long count);
        void OnJamComboIncremented(const Chart::NoteEvent& ev, Accuracy acc, unsigned long long jamCombo);

        void OnExitButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        using ImageMap = std::unordered_map<Chart::Channel, Gx::Image*>;
        using InputStateMap = std::unordered_map<Chart::Channel, bool>;
        using AnimationMap = std::unordered_map<Chart::Channel, Gx::Animation*>;
        using AvatarMap = std::unordered_map<unsigned int, Avatar*>;

        PlayingService& m_service;
        SessionContext& m_session;
        RoomContext& m_room;
        GameContext m_context;
        GameConfig& m_config;
        LifeSystem& m_lifeSystem;
        ItemFactory& m_items;

        ChartRenderer m_renderer;
        AvatarMap m_avatars;
        AnimationMap m_noteClicks;
        AnimationMap m_longNoteEffects;
        Avatar* m_self;
        ComboCounter* m_comboCounter;
        JudgementIndicator* m_judgementIndicator;
        std::vector<Gx::Node*> m_buffers;

        Gx::InputField* m_chatBox;
        ImageMap m_keyDowns, m_keyEffects, m_guideKeyEffects;
        InputStateMap m_inputStates;
        InputStateMap m_guideKeyEffectStates;
        std::unordered_map<Chart::Channel, float> m_guideKeyEffectDeltas;

        std::unordered_map<std::uint8_t, UserState> m_states{};

        unsigned int m_viewport;
    };
}
