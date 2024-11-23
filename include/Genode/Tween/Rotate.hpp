#pragma once

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
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        Transformable* m_target;
        float m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}
