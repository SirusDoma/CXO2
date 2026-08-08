#pragma once

#include <Subscriptions.hpp>

#include <CXO2/Events/StateEvents.hpp>
#include <CXO2/Services/PluginService.hpp>
#include <CXO2/States/State.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace O2CX
{
    class StatelessExtension
    {
    public:
        virtual ~StatelessExtension() = default;

        void Attach(const Cx::AppPluginContext& app)
        {
            m_events = &app.Events;
            OnAttached(app);
        }

        void Detach()
        {
            OnDetached();
            m_subscriptions.Clear();
            m_events = nullptr;
        }

    protected:
        virtual void OnAttached(const Cx::AppPluginContext& app) = 0;
        virtual void OnDetached() {}

        Gx::EventDispatcher& GetEvents() const { return *m_events; }
        SubscriptionSet& GetSubscriptions() { return m_subscriptions; }

    private:
        Gx::EventDispatcher* m_events{nullptr};
        SubscriptionSet m_subscriptions;
    };
}
