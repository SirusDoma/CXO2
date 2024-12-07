#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class StateBulletin : public State
{
public:
    explicit StateBulletin(Gx::AudioMixer& mixer);
    void Initialize() override;

private:
    Gx::AudioMixer& m_mixer;
};
