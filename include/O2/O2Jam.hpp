#ifndef O2JAM_HPP
#define O2JAM_HPP

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <O2/Character/ItemFactory.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

protected:
    virtual void OnStart();
    virtual void OnFocusChanged(bool focus);

private:
    Gx::ResourceManager m_resources = Gx::ResourceManager();
    Gx::Mixer m_mixer = Gx::Mixer();
    ItemFactory m_itemFactory;
};

#endif
