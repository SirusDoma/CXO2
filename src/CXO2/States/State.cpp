#include <CXO2/States/State.hpp>
#include <CXO2/States/StateAvi.hpp>
#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateMusicShop.hpp>
#include <CXO2/States/StateBulletin.hpp>
#include <CXO2/States/StatePayment.hpp>
#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/States/StateResult.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Events/StateEvents.hpp>

#include <CXO2/Network/Requests/SyncMusicDownloadRequest.hpp>
#include <CXO2/Services/MusicDownloaderService.hpp>
#include <CXO2/Services/NetworkService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Messages/Application.hpp>

#include <Genode/Events/EventDispatcher.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Cursor.hpp>

#include <memory>

namespace Cx
{
    using namespace Constants::Identifiers;

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

    State::ExtensionTerminal::ExtensionTerminal(State& owner) :
        m_owner(owner)
    {
    }

    bool State::ExtensionTerminal::Initialize()
    {
        return m_owner.InternalInitialize();
    }

    void State::ExtensionTerminal::Finalize()
    {
        m_owner.InternalFinalize();
    }

    void State::ExtensionTerminal::Update(const sf::Time& delta)
    {
        m_owner.InternalUpdate(delta);
    }

    Gx::RenderStates State::ExtensionTerminal::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        return m_owner.InternalRender(surface, states);
    }

    bool State::ExtensionTerminal::Input(const sf::Event& ev)
    {
        return m_owner.InternalInput(ev);
    }

    StateExtension& State::Attach(StateExtensionPtr extension)
    {
        auto& ext = *extension;
        ext.m_state = this;
        ext.m_next  = &m_terminal;

        if (!m_extensions.empty())
            m_extensions.back()->m_next = &ext;

        m_extensions.push_back(std::move(extension));
        return ext;
    }

    StateExtension& State::GetNextExtension()
    {
        return m_extensions.empty() ? static_cast<StateExtension&>(m_terminal) : *m_extensions.front();
    }

    const StateExtension& State::GetNextExtension() const
    {
        return m_extensions.empty() ? static_cast<const StateExtension&>(m_terminal) : *m_extensions.front();
    }

    bool State::Initialize(StateEventArgs&& args)
    {
        if (Dispatch(StateEvents::OnInitialize, args))
            return false;

        m_extensions.clear();
        Dispatch(StateEvents::OnExtend, StateEventArgs{GetName()});

        return GetNextExtension().Initialize();
    }

    bool State::InternalInitialize()
    {
        if (m_persistentDialog)
        {
            Invoke([this]
            {
                if (!m_persistentDialog)
                    return;

                const auto pending = *m_persistentDialog;
                ShowDialog(pending.Content, pending.Style, true, pending.Backdrop, pending.Callback);
            });
        }

        Require<MusicDownloaderService>().SetDownloadCompletedCallback([this] (const std::uint16_t musicID)
        {
            if (const auto shop = dynamic_cast<StateMusicShop*>(this))
            {
                shop->OnDownloadCompleted(musicID);
                return;
            }

            if (dynamic_cast<StateAvi*>(this) || dynamic_cast<StatePlanet*>(this))
                return;

            Require<NetworkService>().Dispatch(SyncMusicDownloadRequest{musicID},nullptr);
        });

        Gx::Scene::Initialize();
        m_tempResources->Clear();

        return true;
    }

    void State::Finalize()
    {
        Require<MusicDownloaderService>().SetDownloadCompletedCallback(nullptr);

        for (const auto dialog : {m_dialogInfo, m_dialog1, m_dialog2})
        {
            if (dialog->IsShown())
                dialog->Dismiss();
        }

        GetNextExtension().Finalize();
        Dispatch(StateEvents::OnFinalize, StateEventArgs{GetName()});
        m_extensions.clear();

        Require<Gx::AudioMixer>().Reset(true);

        m_exitPrompted = false;
        m_exitDialog->Dismiss();
    }

    void State::InternalFinalize()
    {
        Scene::Finalize();
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

        m_cancelSound = Instantiate<sf::Sound>(Sound::Effects::EF_03, ResourceScope::Shared);
        m_popupSound  = Instantiate<sf::Sound>(Sound::Effects::EF_06, ResourceScope::Shared);

        loaded = true;
    }

    Gx::Dialog* State::GetDialog(const DialogStyle style)
    {
        if (style == DialogStyle::OkCancel)
            return m_dialog1;

        if (style == DialogStyle::YesNo)
            return m_dialog2;

        return m_dialogInfo;
    }

    void State::ShowDialog(const sf::String& content, const DialogStyle style, std::function<void(bool)> callback)
    {
        ShowDialog(content, style, true, false, std::move(callback));
    }

    void State::ShowDialog(const sf::String& content, const DialogStyle style, const bool persist, const bool backdrop, std::function<void(bool)> callback)
    {
        const auto dialog = GetDialog(style);
        const auto label  = dialog->GetLabel();
        label->SetVisible(true);

        if (!callback)
            callback = [] (bool) {};

        if (persist)
            m_persistentDialog = PersistentDialog{content, style, backdrop, callback};
        else if (m_persistentDialog && m_persistentDialog->Style == style)
            m_persistentDialog.reset();

        dialog->SetAcceptCallback([=] (auto&, auto&)
        {
            if (m_persistentDialog && m_persistentDialog->Style == style)
                m_persistentDialog.reset();

            callback(true);
        });

        dialog->SetCancelCallback([=] (auto&, auto&)
        {
            if (m_persistentDialog && m_persistentDialog->Style == style)
                m_persistentDialog.reset();

            callback(false);
        });

        auto ctx        = Gx::DialogPresentationContext();
        ctx.Bounds      = sf::FloatRect{{0, 0}, GetDefaultView().getSize() };
        ctx.Prompt      = content;
        ctx.UseBackdrop = backdrop;

        Present(*dialog, ctx);
    }

    void State::ShowDialog(Gx::Node& content, const DialogStyle style, const bool backdrop, std::function<void(bool)> callback)
    {
        const auto dialog = GetDialog(style);
        if (m_persistentDialog && m_persistentDialog->Style == style)
            m_persistentDialog.reset();

        const auto label        = dialog->GetLabel();
        const auto acceptButton = dialog->GetAcceptButton();
        const auto cancelButton = dialog->GetCancelButton();

        dialog->ClearChildren();
        dialog->AddChild(*label, *acceptButton, *cancelButton, content);
        label->SetVisible(false);

        if (!callback)
            callback = [] (bool) {};

        dialog->SetAcceptCallback([=] (auto&, auto&) { callback(true); });
        dialog->SetCancelCallback([=] (auto&, auto&) { callback(false); });

        auto ctx        = Gx::DialogPresentationContext();
        ctx.Bounds      = sf::FloatRect{{0, 0}, GetDefaultView().getSize() };
        ctx.UseBackdrop = backdrop;

        Present(*dialog, ctx);
    }

    void State::Announce(const sf::String& content)
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
                    ShowDialog(Constants::Messages::Application::Display::WINDOW_CURSOR_CONFIRM, DialogStyle::OkCancel, [&] (const bool response)
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

                    ShowDialog(Constants::Messages::Application::Display::IMAGE_CURSOR_ENABLED, DialogStyle::Information);
                }
            }
            else if (ev.code == sf::Keyboard::Key::F7)
            {
                if (config.UseFx)
                {
                    ShowDialog(Constants::Messages::Application::Display::ENABLE_3D_CONFIRM, DialogStyle::YesNo, [&config] (const bool confirm)
                    {
                        if (confirm)
                        {
                            config.UseFx = true;
                            config.Save();
                        }
                    });
                }
                else
                {
                    ShowDialog(Constants::Messages::Application::Display::DISABLE_3D_CONFIRM, DialogStyle::YesNo, [&config] (const bool confirm)
                    {
                        if (confirm)
                        {
                            config.UseFx = false;
                            config.Save();
                        }
                    });
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

    void State::ExitGame(const sf::String& prompt, sf::FloatRect bounds)
    {
        if (m_popupSound)
        {
            auto& mixer = Require<Gx::AudioMixer>();
            mixer.Play(*m_popupSound, Sound::Channel::SFX);
        }

        m_exitDialog->SetAcceptCallback([&] (auto&, auto&)
        {
            m_exitPrompted = true;
            Gx::Application::Instance().Close();
        });

        m_exitDialog->SetCancelCallback([&] (auto&, auto&)
        {
            auto& mixer = Gx::Application::Instance().GetModule<Gx::Context>().Require<Gx::AudioMixer>();

            mixer.Play(*m_cancelSound, Sound::Channel::SFX);
            m_exitPrompted = false;
        });

        auto ctx        = Gx::DialogPresentationContext();
        ctx.Bounds      = bounds.size == sf::Vector2f{} ? sf::FloatRect{{0, 0}, GetDefaultView().getSize() } : bounds;
        ctx.Prompt      = prompt;
        ctx.UseBackdrop = true;
        Present(*m_exitDialog, ctx);
    }

    void State::Update(const sf::Time& delta)
    {
        auto ev = StateUpdateEventArgs{{}, delta};
        if (Dispatch(StateEvents::OnUpdate, ev))
            return;

        GetNextExtension().Update(ev.Delta);
    }

    void State::InternalUpdate(const sf::Time& delta)
    {
        Scene::Update(delta);
    }

    bool State::Input(const sf::Event& ev)
    {
        auto data = StateInputEventArgs{{}, ev};
        if (Dispatch(StateEvents::OnInput, data))
            return false;

        return GetNextExtension().Input(data.Input);
    }

    bool State::InternalInput(const sf::Event& ev)
    {
        return Scene::Input(ev);
    }

    Gx::RenderStates State::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        auto ev = StateRenderEventArgs{{}, surface, states};
        if (Dispatch(StateEvents::OnRender, ev))
            return states;

        return GetNextExtension().Render(surface, states);
    }

    Gx::RenderStates State::InternalRender(Gx::RenderSurface& surface, Gx::RenderStates states) const
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
            if (Require<MusicDownloaderService>().IsDownloading())
                ExitGame(Constants::Messages::Application::Exit::CONFIRM_WHILE_DOWNLOADING);
            else
                ExitGame(Constants::Messages::Application::Exit::CONFIRM);

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
