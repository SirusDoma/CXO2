#include <OTwo/States/Components/Playing/Equalizer.hpp>

Equalizer::Equalizer() :
    Equalizer(0, 0)
{
}

Equalizer::Equalizer(const int count, const float spacing) :
    Gx::List(1, 0, count, spacing)
{
    Gx::List::SetOrder(Order::Horizontal);
    Gx::List::SetBatchingEnabled(true);
}

void Equalizer::OnControlChildAdded(Gx::Control *control)
{
    if (const auto gauge = dynamic_cast<Gx::Gauge*>(control); gauge)
    {
        gauge->SetValue(1);
        m_gauges.push_back(gauge);
    }
}

void Equalizer::OnControlChildRemove(Gx::Control *control)
{
    if (const auto gauge = dynamic_cast<Gx::Gauge*>(control); gauge)
        m_gauges.erase(std::remove(m_gauges.begin(), m_gauges.end(), gauge), m_gauges.end());
}
