#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>
#include <Genode/Tween/Tween.hpp>

namespace Gx
{
    class Move : public Tween<Transformable>
    {
    public:
        Move(Transformable& target, sf::Vector2f position, const sf::Time& duration, MotionFunc motion = Motion::Linear);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        sf::Vector2f m_start, m_end, m_diff;
    };
}
