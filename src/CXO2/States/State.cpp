#include <CXO2/States/State.hpp>
#include <CXO2/States/StateAvi.hpp>
#include <CXO2/States/StateBulletin.hpp>
#include <CXO2/States/StatePayment.hpp>
#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/States/StateResult.hpp>
#include <CXO2/Config/GameConfig.hpp>

#include <CXO2/Network/NetworkAdapter.hpp>
#include <CXO2/StringTable/Identifiers/Sound.hpp>

#include <Genode/IO/ResourceManager.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Cursor.hpp>

#include <memory>

namespace Cx
{
    using namespace StringTable::Identifiers;

    State::State() :
        State(typeid(this).name())
    {
        LoadCommonResources();
    }

    State::State(const std::string& name) :
        Gx::Scene(name),
        m_resources(std::make_unique<Gx::ResourceManager>()),
        m_tempResources(std::make_unique<Gx::ResourceManager>())
    {
        LoadCommonResources();
    }

    State::State(const std::string& name, Gx::ResourceManager& resources) :
        Gx::Scene(name),
        m_resources(&resources),
        m_tempResources(std::make_unique<Gx::ResourceManager>())
    {
        LoadCommonResources();
    }

    void State::Finalize()
    {
        Scene::Finalize();

        // Require<NetworkAdapter>().ResetQueue();
        Require<Gx::AudioMixer>().Reset(true);

        m_exitPrompted = false;
        m_exitDialog->Dismiss();
    }

    void State::Initialize()
    {
        Gx::Scene::Initialize();
        m_tempResources->Clear();
    }

    void State::LoadCommonResources()
    {
        static bool loaded = false;
        if (loaded)
            return;

        m_exitPrompted    = false;
        if (Gx::FileSystem::Contains("ControlList_Interface.txt"))
        {
            m_dialogInfo   = Instantiate<Gx::Dialog>("ControlList/Dialog/Information.json", ResourceScope::Shared);
            m_dialog1      = Instantiate<Gx::Dialog>("ControlList/Dialog/Question1.json", ResourceScope::Shared);
            m_dialog2      = Instantiate<Gx::Dialog>("ControlList/Dialog/Question2.json", ResourceScope::Shared);
            m_dialogNotice = Instantiate<Gx::Dialog>("ControlList/Dialog/Notice.json", ResourceScope::Shared);
            m_exitDialog   = Instantiate<Gx::Dialog>("ControlList/Dialog/Question2.json", ResourceScope::Shared);
        }
        else
        {
            m_dialogInfo   = Instantiate<Gx::Dialog>("Interface/Dialog/Information.json", ResourceScope::Shared);
            m_dialog1      = Instantiate<Gx::Dialog>("Interface/Dialog/Question1.json", ResourceScope::Shared);
            m_dialog2      = Instantiate<Gx::Dialog>("Interface/Dialog/Question2.json", ResourceScope::Shared);
            m_dialogNotice = Instantiate<Gx::Dialog>("Interface/Dialog/Notice.json", ResourceScope::Shared);
            m_exitDialog   = Instantiate<Gx::Dialog>("Interface/Dialog/Question2.json", ResourceScope::Shared);
        }

        const auto center    = GetApplication().GetView().getCenter();
        const unsigned int x = static_cast<unsigned int>(center.x - (m_dialogNotice->GetLocalBounds().size.x / 2.f));
        const unsigned int y = static_cast<unsigned int>(center.y - (m_dialogNotice->GetLocalBounds().size.y / 2.f));

        m_dialogNotice->SetVisible(false);
        m_dialogNotice->SetOrigin(0.f, 0.f);
        m_dialogNotice->SetPosition(x, y);
        m_dialogNotice->GetLabel()->SetLocalBounds(sf::FloatRect{{}, {152, 300}});

        m_cancelSound = Instantiate<sf::Sound>(Sound::Effects::EF_03, ResourceScope::Shared);
        m_popupSound  = Instantiate<sf::Sound>(Sound::Effects::EF_06, ResourceScope::Shared);

        loaded = true;
    }

    void State::ShowDialog(const sf::String& content, const DialogStyle style, const bool backdrop, std::function<void(bool)> callback)
    {
        auto dialog = m_dialogInfo;
        if (style == DialogStyle::OkCancel)
            dialog = m_dialog1;
        else if (style == DialogStyle::YesNo)
            dialog = m_dialog2;

        const auto label = dialog->GetLabel();
        label->SetVisible(true);

        if (!callback)
            callback = [] (bool) {};

        dialog->SetAcceptCallback([=] () { callback(true); });
        dialog->SetCancelCallback([=] () { callback(false); });

        auto ctx        = Gx::DialogPresentationContext();
        ctx.Bounds      = sf::FloatRect{{0, 0}, GetDefaultView().getSize() };
        ctx.Prompt      = content;
        ctx.UseBackdrop = backdrop;

        Present(*dialog, ctx);
    }

    void State::ShowDialog(Gx::Node& content, const DialogStyle style, const bool backdrop, std::function<void(bool)> callback)
    {
        auto dialog = m_dialogInfo;
        if (style == DialogStyle::OkCancel)
            dialog = m_dialog1;
        else if (style == DialogStyle::YesNo)
            dialog = m_dialog2;

        const auto label        = dialog->GetLabel();
        const auto acceptButton = dialog->GetAcceptButton();
        const auto cancelButton = dialog->GetCancelButton();

        dialog->ClearChildren();
        dialog->AddChild(*label, *acceptButton, *cancelButton, content);
        label->SetVisible(false);

        if (!callback)
            callback = [] (bool) {};

        dialog->SetAcceptCallback([=] () { callback(true); });
        dialog->SetCancelCallback([=] () { callback(false); });

        auto ctx        = Gx::DialogPresentationContext();
        ctx.Bounds      = sf::FloatRect{{0, 0}, GetDefaultView().getSize() };
        ctx.UseBackdrop = backdrop;

        Present(*dialog, ctx);
    }

    void State::Announce(const std::string& content)
    {
        m_dialogNotice->SetVisible(true);
        m_dialogNotice->SetPromptString(content);

        m_noticeTimer.restart();
    }

    void State::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        Scene::OnKeyPressed(ev);

        const auto& director = GetDirector();

        if (!director.IsPresenting<StateAvi>() &&
            !director.IsPresenting<StatePayment>() &&
            !director.IsPresenting<StateBulletin>() &&
            !director.IsPresenting<StatePlaying7K>())
        {
            auto& config = Require<GameConfig>();
            if (ev.code == sf::Keyboard::Key::F8 && !director.IsPresenting<StateResult>())
            {
                if (!config.UseWindowCursor)
                {
                    ShowDialog("Would you like to change to window\ncursor?\nIf you cannot see the cursor press\nF8 again", DialogStyle::OkCancel, false, [&] (const bool response)
                    {
                        if (response)
                        {
                            config.UseWindowCursor = true;
                            config.Save();
                            if (const auto cursor = GetApplication().GetCursor(); cursor)
                            {
                                cursor->SetEnabled(!config.UseWindowCursor);
                                GetApplication().InvalidateCursor();
                            }
                        }
                    });
                }
                else
                {
                    config.UseWindowCursor = false;
                    config.Save();
                    if (const auto cursor = GetApplication().GetCursor(); cursor)
                    {
                        cursor->SetEnabled(!config.UseWindowCursor);
                        GetApplication().InvalidateCursor();
                    }

                    ShowDialog("Changed to image cursor.\nTo change back to window cursor press F8\nagain", DialogStyle::Information);
                }
            }
            else if (ev.code == sf::Keyboard::Key::F9)
            {
                config.UseEqualizer = !config.UseEqualizer;
                config.Save();
                if (config.UseEqualizer)
                    ShowDialog("Activating equalizer.\n( To deactive press F9 again. )", DialogStyle::Information);
                else
                    ShowDialog("Equalizer deactivated.\n( To active press F9 again. )", DialogStyle::Information);
            }
            else if (ev.code == sf::Keyboard::Key::F10)
            {
                config.UseVsync = !config.UseVsync;
                config.Save(); // TODO: Persist VSync

                sf::RenderWindow& window = GetApplication();
                window.setVerticalSyncEnabled(config.UseVsync);

                if (config.UseVsync)
                    ShowDialog("Vsync enabled.\n( Press F10 again to disable. )", DialogStyle::Information);
                else
                    ShowDialog("Vsync disabled.\n( Press F10 again to enable. )", DialogStyle::Information);
            }
        }
    }

    Gx::RenderStates State::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        auto result = Scene::Render(surface, states);
        if (m_dialogNotice->IsVisible() && m_noticeTimer.getElapsedTime() < sf::seconds(15))
        {
            if (const auto updatable = dynamic_cast<Updatable*>(m_dialogNotice))
                updatable->Update(states.Delta);

            surface.Render(*m_dialogNotice, states);
        }
        else if (m_noticeTimer.isRunning())
        {
            m_dialogNotice->SetVisible(false);
            m_noticeTimer.reset();
        }

        return result;
    }

    bool State::OnAppClose()
    {
        if (!m_exitPrompted)
        {
            if (m_popupSound)
            {
                auto& mixer = Require<Gx::AudioMixer>();
                mixer.Play(*m_popupSound, Sound::Channel::SFX);
            }

            m_exitDialog->SetAcceptCallback([&]
            {
                m_exitPrompted = true;
                Gx::Application::Instance().Close();
            });

            m_exitDialog->SetCancelCallback([&]
            {
                auto& mixer = Gx::Application::Instance().GetContext().Require<Gx::AudioMixer>();

                mixer.Play(*m_cancelSound, Sound::Channel::SFX);
                m_exitPrompted = false;
            });

            auto ctx        = Gx::DialogPresentationContext();
            ctx.Bounds      = sf::FloatRect{{0, 0}, GetDefaultView().getSize() };
            ctx.Prompt      = "Do you really want to exit?";
            ctx.UseBackdrop = true;
            Present(*m_exitDialog, ctx);

            return false;
        }

        return Scene::OnAppClose();
    }

    Gx::ResourceManager& State::GetResources(const ResourceScope scope)
    {
        switch (scope)
        {
            case ResourceScope::Immediate: return *m_tempResources;
            case ResourceScope::Local:     return *m_resources;
            default:                       return Require<Gx::ResourceManager>();
        }
    }
}
