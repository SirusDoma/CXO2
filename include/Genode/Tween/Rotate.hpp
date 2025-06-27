#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>
#include <Genode/Tween/Tween.hpp>

namespace Gx
{
    class Rotate : public Tween<Transformable>
    {
    public:
        Rotate(Transformable& target, float rotation, const sf::Time& duration, MotionFunc motion = Motion::Linear);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        float m_start, m_end, m_diff;
    };
}
