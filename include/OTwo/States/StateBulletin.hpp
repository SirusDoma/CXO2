#pragma once

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateBulletin : public State
{
public:
    explicit StateBulletin(Gx::AudioMixer& mixer);
    void Initialize() override;

private:
    Gx::AudioMixer& m_mixer;
};
