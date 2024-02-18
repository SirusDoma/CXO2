#include <OTwo/States/State.hpp>

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
    m_sound(),
    m_prompted(false)
{
}

void State::Initialize()
{
    auto& mixer = GetApplication().Require<Gx::Mixer>();

    m_dialog = Create<Gx::Dialog>("Interface/Metadata/Dialog/Question2.json");
    m_sound  = mixer.Create<sf::Sound>("Interface/Metadata/Common/ExitSound.json");
    m_dialog->SetAcceptCallback([=] {
        m_prompted = true;
        GetDirector().GetApplication().Close();
    });
}

bool State::Close(bool quit)
{
    if (quit && !m_prompted && m_dialog)
    {
        auto& mixer = GetApplication().Require<Gx::Mixer>();
        mixer.Play(m_sound, "SFX");

        m_dialog->Show(this, "Do you really want to exit?", true);
        return false;
    }

    return Scene::Close(quit);
}
