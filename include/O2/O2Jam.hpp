#ifndef O2JAM_HPP
#define O2JAM_HPP

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

protected:
    virtual void OnStart();

private:
    Gx::ResourceManager m_resources = Gx::ResourceManager();
    Gx::Mixer m_mixer = Gx::Mixer();
};

#endif
