#include <O2/Scenes/State.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

State::State() :
    State::State(typeid(this).name())
{
}

State::~State()
{
}

State::State(const std::string &name) :
    Scene(name),
    m_dialog(),
    m_prompted(false)
{
    m_dialog = std::unique_ptr<Gx::Dialog>(Gx::ResourceManager::Instance()->Create<Gx::Dialog>("Metadata\\Dialog\\Question2.json"));
    m_dialog->SetAcceptCallback([=] {
        m_prompted = true;
        Gx::Application::Instance()->Close();
    });
}

bool State::Close(bool quit)
{
    if (quit && !m_prompted && m_dialog)
    {
        m_dialog->Show(this, "Do you really want to exit?", true);
        return false;
    }

    return Scene::Close(quit);
}
