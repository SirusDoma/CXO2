#pragma once

#include <SFML/System/Time.hpp>

#include <Genode/Tasks/Task.hpp>
#include <Genode/Entities/Colorable.hpp>

namespace Gx
{
    class Fade : public Task
    {
    public:
        Fade() = default;
        Fade(Colorable& target, std::uint8_t opacity, const sf::Time& duration);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        Colorable* m_target;
        std::int16_t m_start, m_end, m_diff;
        sf::Time m_duration;
    };
}
