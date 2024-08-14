#include <Genode/Tasks/Step.hpp>

namespace Gx
{
    Step::Step() :
        Delay(),
        m_stepDuration(sf::Time::Zero),
        m_elapsed(sf::Time::Zero)
    {
    }

    Step::Step(const sf::Time duration, const sf::Time stepDuration, const std::function<void(const Step*, double)> &update) :
        Delay(duration),
        m_stepDuration(stepDuration),
        m_callback(update)
    {
    }

    void Step::Update(const double delta)
    {
        Delay::Update(delta);
        if (GetState() == TaskState::Initial || GetState() == TaskState::Running)
        {
            m_elapsed += sf::milliseconds(delta);
            if (m_callback && m_elapsed >= m_stepDuration)
            {
                m_callback(this, m_elapsed.asMilliseconds());
                m_elapsed -= m_stepDuration;
            }
        }
    }

    void Step::Complete()
    {
        Delay::Complete();
        if (m_callback)
            m_callback(this, m_elapsed.asMilliseconds());

        m_elapsed = sf::Time::Zero;
    }

    void Step::Reset()
    {
        Delay::Reset();
        m_elapsed = sf::Time::Zero;
    }
}
