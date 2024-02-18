#ifndef O2JAM_STATE_LOGO_HPP
#define O2JAM_STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateAvi : public Gx::Scene
{
public:
    virtual void Initialize();
    virtual bool Close(bool quit = false);

private:
    Gx::ResourcePtr<sf::Music> m_bgm;
};

#endif