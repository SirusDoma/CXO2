#pragma once

#include <CXO2/Events/Interceptable.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace Cx
{
    class Dispatchable
    {
    protected:
        explicit Dispatchable(Gx::EventDispatcher& events) :
            m_events(events)
        {
        }

        ~Dispatchable() = default;

        template <typename TKey, typename TSender, typename... TArgs, typename... UArgs>
        bool Dispatch(const Gx::Event<TKey, TSender, TArgs...>& event, UArgs&&... args)
        {
            m_events.Dispatch(event, static_cast<TSender&>(*this), args...);
            return (false || ... || args.Handled);
        }

    private:
        Gx::EventDispatcher& m_events;
    };
}
