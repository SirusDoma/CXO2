#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class ScoreTracker;
class SessionContext;
class StateResult : public State
{
public:
    StateResult(Gx::AudioMixer& mixer, SessionContext& session, const ScoreTracker& scoreTracker);
    void Initialize() override;

private:
    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    const ScoreTracker& m_scoreTracker;
};
