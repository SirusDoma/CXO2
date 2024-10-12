#ifndef O2JAM_STATE_BULLETIN_HPP
#define O2JAM_STATE_BULLETIN_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateBulletin : public State
{
public:
    explicit StateBulletin(Gx::Mixer& mixer);
    void Initialize() override;

private:
    Gx::Mixer& m_mixer;
};

#endif