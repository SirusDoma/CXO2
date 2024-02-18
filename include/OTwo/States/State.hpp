#ifndef O2JAM_STATE_HPP
#define O2JAM_STATE_HPP

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Dialog.hpp>

#include <memory>

class State : public Gx::Scene
{
public:
    State();
    State(const std::string& name);
    virtual ~State();

    virtual bool Close(bool quit = false);

protected:
    virtual void Initialize();

private:
    Gx::Dialog* m_dialog;
    sf::Sound*  m_sound;
    bool m_prompted;
};

#endif