#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class StateAvi : public State
{
public:
    explicit StateAvi(Gx::AudioMixer& mixer);

    void Initialize() override;
    bool OnAppClose() override;

private:
    Gx::AudioMixer& m_mixer;
};
