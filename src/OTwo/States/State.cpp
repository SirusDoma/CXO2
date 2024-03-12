#include <OTwo/States/State.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <memory>

State::State() :
    State::State(typeid(this).name())
{
    LoadCommonResources();
}

State::State(State &state) :
    Gx::Scene::Scene(state)
{
    LoadCommonResources();

    SetParent(state.GetParent());
    SetName(state.GetName());
    SetTag(state.GetTag());

    SetOrigin(state.GetOrigin());
    SetPosition(state.GetPosition());
    SetScale(state.GetScale());
    SetRotation(state.GetRotation());

    m_tempResources = std::make_unique<Gx::ResourceManager>();
    if (state.m_resources)
        m_resources = std::move(state.m_resources);

    for (auto child : state.GetChildren())
        AddChild(child);
}

State::State(const std::string &name) :
    Gx::Scene(name),
    m_resources(std::make_unique<Gx::ResourceManager>()),
    m_tempResources(std::make_unique<Gx::ResourceManager>())
{
    LoadCommonResources();
}

State::State(const std::string &name, Gx::ResourceManager &resources) :
    Gx::Scene(name),
    m_resources(&resources),
    m_tempResources(std::make_unique<Gx::ResourceManager>())
{
    LoadCommonResources();
}

State::~State()
{
    Require<Gx::Mixer>().StopAll();
}

void State::Initialize()
{
    m_tempResources->Clear();
}

void State::LoadCommonResources()
{
    m_prompted    = false;
    m_dialog      = Load<Gx::Dialog>("Interface/Metadata/Dialog/Question2.json", ResourceScope::Shared);
    m_popupSound  = Load<sf::Sound>("Interface/Metadata/Common/ExitSound.json", ResourceScope::Shared);
    m_cancelSound = Load<sf::Sound>("Interface/Metadata/Common/CancelSound1.json", ResourceScope::Shared);

    static bool wired = false;
    if (!wired)
    {
        m_dialog->SetAcceptCallback([&] {
            m_prompted = true;
            Gx::Application::Instance().Close();
        });
        m_dialog->SetCancelCallback([&] {
            auto& mixer = Gx::Application::Instance().Require<Gx::Mixer>();
            mixer.Play(m_cancelSound, "SFX");

            m_prompted = false;
        });
    }

    wired = true;
}

bool State::Close(bool quit)
{
    if (quit && !m_prompted && m_dialog)
    {
        auto& mixer = Require<Gx::Mixer>();
        mixer.Play(m_popupSound, "SFX");

        m_dialog->Show(this, "Do you really want to exit?", true);
        return false;
    }

    return Scene::Close(quit);
}

Gx::ResourceManager &State::GetLocalResources() const
{
    return *m_resources.get();
}
