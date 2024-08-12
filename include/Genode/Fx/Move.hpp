#ifndef GENODE_FX_MOVE_HPP
#define GENODE_FX_MOVE_HPP

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>
#include <Genode/System/Primitives.hpp>

namespace Gx
{
    class Move : public Task
    {
    public:
        Move(Transformable* target, sf::Vector2f position, const sf::Time& duration);

        void Update(const double delta) override;
        void Reset() override;

    private:
        Transformable* m_target;
        sf::Vector2f m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}

#endif