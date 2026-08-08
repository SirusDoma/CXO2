#pragma once

#include <Subscriptions.hpp>

#include <CXO2/Events/PlayingEvents.hpp>
#include <CXO2/States/StateExtension.hpp>

#include <Genode/Events/EventDispatcher.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/UI/Label.hpp>

#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>

namespace O2CX
{
    class AccuracyMeterExtension : public Cx::StateExtension
    {
    public:
        explicit AccuracyMeterExtension(Gx::EventDispatcher& events);

    protected:
        bool Initialize() override;

    private:
        void BuildMeter();
        void OnJudgement(const Cx::PlayingJudgementEventArgs& ev);

        Gx::EventDispatcher& m_events;
        SubscriptionSet m_subscriptions;
        std::vector<std::unique_ptr<Gx::Node>> m_parts;
        Gx::Rectangle* m_marker{nullptr};
        Gx::Label* m_latencyText{nullptr};
        sf::Vector2f m_center;
        float m_barWidth{224.f};
        float m_textTop{0.f};
    };
}
