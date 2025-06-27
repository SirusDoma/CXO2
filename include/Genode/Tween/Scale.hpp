#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>
#include <Genode/Tween/Tween.hpp>

#include <SFML/System/Vector2.hpp>

namespace Gx
{
    class Scale : public Tween<Transformable>
    {
    public:
        Scale(Transformable& target, sf::Vector2f scale, const sf::Time& duration, MotionFunc motion = Motion::Linear);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        sf::Vector2f m_start, m_end, m_diff;
    };
}
