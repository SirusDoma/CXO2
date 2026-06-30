#pragma once

#include <CXO2/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

namespace Cx
{
    class StateBulletin : public State
    {
    public:
        explicit StateBulletin(Gx::AudioMixer& mixer);
        void Initialize() override;

    private:
        Gx::AudioMixer& m_mixer;
    };
}
