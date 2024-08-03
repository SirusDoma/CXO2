#include <OTwo/States/State.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <Genode/UI/Button.hpp>

#include <memory>

State::State() :
    State(typeid(this).name())
{
    LoadCommonResources();
}

State::State(State &&state) :
    Scene(state)
{
    LoadCommonResources();

    State::SetParent(state.GetParent());
    SetName(state.GetName());
    SetTag(state.GetTag());

    SetOrigin(state.GetOrigin());
    SetPosition(state.GetPosition());
    SetScale(state.GetScale());
    SetRotation(state.GetRotation());

    m_tempResources = std::make_unique<Gx::ResourceManager>();
    if (state.m_resources)
        m_resources = std::move(state.m_resources);

    for (const auto child : state.GetChildren())
        State::AddChild(child);
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
    static bool loaded = false;
    if (loaded)
        return;

    m_prompted    = false;
    m_dialogInfo  = Instantiate<Gx::Dialog>("Interface/Dialog/Information.json", ResourceScope::Shared);
    m_dialog1     = Instantiate<Gx::Dialog>("Interface/Dialog/Question1.json", ResourceScope::Shared);
    m_dialog2     = Instantiate<Gx::Dialog>("Interface/Dialog/Question2.json", ResourceScope::Shared);
    m_cancelSound = Instantiate<sf::Sound>("Interface/Sound/Effect/03.json", ResourceScope::Shared);
    m_popupSound  = Instantiate<sf::Sound>("Interface/Sound/Effect/06.json", ResourceScope::Shared);

    if (m_dialog2)
    {
        m_exitDialog = Instantiate<Gx::Dialog>("Interface/Dialog/Question2.json", ResourceScope::Shared);
        m_exitDialog->SetAcceptCallback([&]
        {
            m_prompted = true;
            Gx::Application::Instance().Close();
        });

        m_exitDialog->SetCancelCallback([&]
        {
            auto &mixer = Gx::Application::Instance().Require<Gx::Mixer>();
            mixer.Play(m_cancelSound, "SFX");

            m_prompted = false;
        });
    }
    loaded = true;
}

void State::ShowDialog(const std::string &content, DialogStyle style, bool backdrop, const std::function<void(bool)> &callback)
{
    auto dialog = m_dialogInfo;
    if (style == DialogStyle::OkCancel)
        dialog = m_dialog1;
    else if (style == DialogStyle::YesNo)
        dialog = m_dialog2;

    dialog->SetAcceptCallback([=] () { callback(true); });
    dialog->SetCancelCallback([=] () { callback(false); });

    dialog->Show(this, content, backdrop);
}

void State::ShowDialog(Gx::Node *content, DialogStyle style, bool backdrop, const std::function<void(bool)> &callback)
{
    auto dialog = m_dialogInfo;
    if (style == DialogStyle::OkCancel)
        dialog = m_dialog1;
    else if (style == DialogStyle::YesNo)
        dialog = m_dialog2;

    auto label        = dialog->GetLabel();
    auto acceptButton = dialog->GetAcceptButton();
    auto cancelButton = dialog->GetCancelButton();

    dialog->ClearChildren();
    dialog->AddChild(label, acceptButton, cancelButton, content);

    dialog->SetAcceptCallback([=] () { callback(true); });
    dialog->SetCancelCallback([=] () { callback(false); });

    dialog->Show(this, std::string(), backdrop);
}

bool State::Close(bool quit)
{
    if (quit && !m_prompted)
    {
        if (m_popupSound)
        {
            auto &mixer = Require<Gx::Mixer>();
            mixer.Play(m_popupSound, "SFX");
        }

        m_exitDialog->Show(this, "Do you really want to exit?", true);
        return false;
    }

    return Scene::Close(quit);
}

Gx::ResourceManager &State::GetLocalResources() const
{
    return *m_resources;
}
