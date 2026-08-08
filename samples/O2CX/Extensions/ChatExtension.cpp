#include <Extensions/ChatExtension.hpp>
#include <Log.hpp>

#include <CXO2/Constants/Identifiers/ChatPanel.hpp>
#include <CXO2/Events/ChatEvents.hpp>
#include <CXO2/Events/StateEvents.hpp>
#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Common/ChatWindow.hpp>

#include <Genode/UI/InputField.hpp>

namespace O2CX
{
    using namespace Cx::Constants::Identifiers;

    namespace
    {
        constexpr auto Keyword = "/o2cx";
    }

    void ChatExtension::OnAttached(const Cx::AppPluginContext& app)
    {
        GetSubscriptions().Add(app.Events.On(Cx::ChatEvents::OnSend, [this] (Cx::ChatPanel& sender, Cx::ChatSendEventArgs& ev)
        {
            if (ev.Message == Keyword)
            {
                ev.Handled = true;
                Log("ChatMod: keyword intercepted");
                ShowResponse(sender, "O2CX is up and running. Happy jamming!");

                return;
            }

            ev.Message += " [o2cx]";
        }));

        GetSubscriptions().Add(app.Events.On(Cx::StateEvents::OnUpdate, [this] (Cx::State& sender, Cx::StateUpdateEventArgs& ev)
        {
            if (m_response && m_timer.getElapsedTime() >= sf::seconds(6.f))
                HideResponse();
        }));

        GetSubscriptions().Add(app.Events.On(Cx::StateEvents::OnFinalize, [this] (Cx::State& sender, Cx::StateEventArgs& ev)
        {
            HideResponse();
        }));
    }

    void ChatExtension::OnDetached()
    {
        HideResponse();
    }

    void ChatExtension::ShowResponse(Cx::ChatPanel& panel, const sf::String& message)
    {
        HideResponse();

        const auto window = panel.FindChild<Cx::ChatWindow>(Resource::ChatPanel::IDC_CHAT_WINDOW);
        const auto input  = panel.FindChild<Gx::InputField>(Resource::ChatPanel::IDC_EDIT_CHAT);
        if (!window || !input || !input->GetFont())
            return;

        m_response = std::make_unique<Gx::Label>();
        m_response->SetName("O2CX_CHAT_RESPONSE");
        m_response->SetFont(*input->GetFont());
        m_response->SetCharacterSize(input->GetCharacterSize());
        m_response->SetColor(sf::Color(255, 214, 68));
        m_response->SetString(message);

        const auto bounds = window->GetLocalBounds();
        m_response->SetPosition(window->GetPosition() + bounds.position + sf::Vector2f(4.f, bounds.size.y - input->GetCharacterSize() - 4.f));

        panel.AddChild(*m_response);
        m_panel = &panel;
        m_timer.restart();
    }

    void ChatExtension::HideResponse()
    {
        if (m_panel && m_response)
            m_panel->RemoveChild(*m_response);

        m_response.reset();
        m_panel = nullptr;
    }
}
