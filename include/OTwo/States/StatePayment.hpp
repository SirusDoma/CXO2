#ifndef O2JAM_STATE_PAYMENT_HPP
#define O2JAM_STATE_PAYMENT_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StatePayment : public State
{
public:
    explicit StatePayment(Gx::Mixer& mixer);
    void Initialize() override;

private:
    Gx::Mixer& m_mixer;
};

#endif