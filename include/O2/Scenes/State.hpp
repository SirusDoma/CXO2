#ifndef STATE_HPP
#define STATE_HPP

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


private:
    std::unique_ptr<Gx::Dialog> m_dialog;
    bool m_prompted;
};

#endif