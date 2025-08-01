#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Services/PlayingService.hpp>

#include <Genode/Audio/AudioMixer.hpp>

class ScoreTracker;
class RoomContext;
class GameContext;
class SessionContext;
class StateResult : public State
{
public:
    StateResult(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& context, const ScoreTracker& scoreTracker, PlayingService& service);

    void Initialize() override;

private:
    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    RoomContext& m_room;
    GameContext& m_context;
    PlayingService& m_service;
    const ScoreTracker& m_scoreTracker;
};
