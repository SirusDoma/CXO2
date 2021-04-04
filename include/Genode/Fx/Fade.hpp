#ifndef GENODE_FX_FADE_HPP
#define GENODE_FX_FADE_HPP

#include <SFML/System/Time.hpp>

#include <Genode/Tasks/Task.hpp>
#include <Genode/Entities/Colorable.hpp>
#include <Genode/System/Primitives.hpp>

namespace Gx
{
    class Fade : public Task
    {
    public:
        Fade(Colorable* target, Uint8 opacity, const sf::Time& duration);

    protected:
        virtual void Update(double delta);
        virtual void Reset();

    private:
        Colorable* m_target;
        Int16 m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}

#endif