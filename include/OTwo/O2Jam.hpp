#ifndef O2JAM_HPP
#define O2JAM_HPP

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Archives/OmcArchive.hpp>
#include <OTwo/Data/UserState.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

    virtual ~O2Jam() {};

protected:
    virtual void Boot();
    virtual void Shutdown();
    virtual void OnFocusChanged(bool focus);
};

#endif
