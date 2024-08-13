#include <OTwo/Core/Chart.hpp>

template<typename T>
T* Chart::AddEvent(const Difficulty diff, T &&ev)
{
    static_assert(std::is_base_of_v<Event, T>, "Parameter must be an Event.");

    m_events[diff].push_back(std::make_unique<T>(ev));
    m_lasts[diff] = std::max(m_lasts[diff], ev.Position);

    return static_cast<T*>(m_events[diff].back().get());
}