#pragma once

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
