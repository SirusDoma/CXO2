#pragma once

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateAvi : public State
{
public:
    explicit StateAvi(Gx::AudioMixer& mixer);

    void Initialize() override;
    bool Close(bool quit) override;

private:
    Gx::AudioMixer& m_mixer;
};
