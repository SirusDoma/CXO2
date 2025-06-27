#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Tween/Motion.hpp>
#include <utility>

namespace Gx
{
    template<typename T>
    class Tween : public Task
    {
    public:
        [[nodiscard]] const sf::Time& GetDuration() const { return m_duration; }
        [[nodiscard]] const T& GetTarget() const { return *m_target; }

    protected:
        Tween(
            T& target,
            const sf::Time& duration,
            MotionFunc motion = Motion::Linear
        ) : m_target(&target), m_duration(duration), m_motion(std::move(motion)) {}


        [[nodiscard]] T& GetTarget() { return *m_target; }
        [[nodiscard]] float Compute(const float t) const { return m_motion(t); }

    private:
        T* m_target;
        sf::Time m_duration;
        MotionFunc m_motion;
    };
}