#ifndef GENODE_FX_ROTATE_HPP
#define GENODE_FX_ROTATE_HPP

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>

namespace Gx
{
    class Rotate : public Task
    {
    public:
        Rotate() = default;
        Rotate(Transformable& target, float rotation, const sf::Time& duration);

        void Update(double delta) override;
        void Reset() override;

    private:
        Transformable* m_target;
        float m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}

#endif