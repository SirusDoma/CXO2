#pragma once

#include <SFML/System/Time.hpp>

#include <Genode/Tween/Tween.hpp>
#include <Genode/Entities/Colorable.hpp>

namespace Gx
{
    class Fade : public Tween<Colorable>
    {
    public:
        Fade(Colorable& target, std::uint8_t opacity, const sf::Time& duration, MotionFunc motion = Motion::Linear);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        std::int16_t m_start, m_end, m_diff;
    };
}
