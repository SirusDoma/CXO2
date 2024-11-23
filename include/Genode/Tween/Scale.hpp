#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Graphics/Transformable.hpp>

#include <SFML/System/Vector2.hpp>

namespace Gx
{
    class Scale : public Task
    {
    public:
        Scale() = default;
        Scale(Transformable& target, sf::Vector2f scale, const sf::Time& duration);

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
