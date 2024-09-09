#ifndef O2JAM_STATE_RESULT_HPP
#define O2JAM_STATE_RESULT_HPP

#include <OTwo/States/State.hpp>

class ScoreTracker;
class SessionContext;
class StateResult : public State
{
public:
    StateResult(Gx::Mixer& mixer, SessionContext& session, const ScoreTracker& scoreTracker);
    void Initialize() override;

private:
    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void OnKeyDown(const sf::Event::KeyEvent ev) override;

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    const ScoreTracker& m_scoreTracker;
};

#endif
