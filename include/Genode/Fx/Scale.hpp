#ifndef GENODE_SCALE_HPP
#define GENODE_SCALE_HPP

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>

#include <SFML/System/Vector2.hpp>

namespace Gx
{
    class Scale : public Task
    {
    public:
        Scale(Transformable* target, sf::Vector2f scale, const sf::Time& duration);

    protected:
        virtual void Update(double delta);
        virtual void Reset();

    private:
        Transformable* m_target;
        sf::Vector2f m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}

#endif