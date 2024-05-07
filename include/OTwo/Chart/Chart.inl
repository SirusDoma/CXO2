#include <OTwo/Chart/Chart.hpp>

template<typename T>
void Chart::AddEvent(Difficulty diff, T &&ev)
{
    static_assert(std::is_base_of_v<Event, T>, "Parameter must be an Event.");

    m_events[diff].push_back(std::make_unique<T>(ev));
}