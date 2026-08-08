#pragma once

#include <StatelessExtension.hpp>

#include <Genode/UI/Label.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/System/String.hpp>

#include <memory>

namespace Cx
{
    class ChatPanel;
    class ChatWindow;
}

namespace O2CX
{
    class ChatExtension : public StatelessExtension
    {
    protected:
        void OnAttached(const Cx::AppPluginContext& app) override;
        void OnDetached() override;

    private:
        void ShowResponse(Cx::ChatPanel& panel, const sf::String& message);
        void HideResponse();

        Cx::ChatPanel* m_panel{nullptr};
        std::unique_ptr<Gx::Label> m_response;
        sf::Clock m_timer;
    };
}
