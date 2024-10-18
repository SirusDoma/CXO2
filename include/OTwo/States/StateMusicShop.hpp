#ifndef O2JAM_STATE_MUSIC_SHOP_HPP
#define O2JAM_STATE_MUSIC_SHOP_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateMusicShop : public State
{
public:
    explicit StateMusicShop(Gx::Mixer& mixer);
    void Initialize() override;

private:
    Gx::Mixer& m_mixer;
};

#endif