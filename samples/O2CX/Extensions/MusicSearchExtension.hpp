#pragma once

#include <Subscriptions.hpp>

#include <CXO2/States/StateExtension.hpp>

#include <Genode/Events/EventDispatcher.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/UI/Label.hpp>

#include <SFML/System/String.hpp>

#include <memory>

namespace Cx
{
    class SelectMusicDialog;
}

namespace O2CX
{
    class MusicSearchExtension : public Cx::StateExtension
    {
    public:
        explicit MusicSearchExtension(Gx::EventDispatcher& events);

    protected:
        void Update(const sf::Time& delta) override;

    private:
        void InjectSearchBox(Cx::SelectMusicDialog& dialog);
        void ApplyFilter();

        SubscriptionSet m_subscriptions;
        Cx::SelectMusicDialog* m_dialog{nullptr};
        std::unique_ptr<Gx::Label> m_searchLabel;
        std::unique_ptr<Gx::InputField> m_searchBox;
        sf::String m_lastQuery;
    };
}
