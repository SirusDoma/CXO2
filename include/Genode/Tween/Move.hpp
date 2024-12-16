#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>

namespace Gx
{
    class Move : public Task
    {
    public:
        Move() = default;
        Move(Transformable& target, sf::Vector2f position, const sf::Time& duration);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        Transformable* m_target;
        sf::Vector2f m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}
