#ifndef O2JAM_HPP
#define O2JAM_HPP

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Archives/OmcArchive.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

    ~O2Jam() override = default;

protected:
    void Boot() override;
    void Shutdown() override;
    void OnFocusChanged(bool focus) override;
};

#endif
