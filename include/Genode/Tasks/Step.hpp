#pragma once

#include <Genode/Tasks/Delay.hpp>

namespace Gx
{
    class Step : public Delay
    {
    public:
        Step();
        explicit Step(sf::Time duration, sf::Time stepDuration, const std::function<void(const Step&, double)> &update);

        void Complete() override;
        void Reset() override;

    protected:
        void Update(double delta) override;

    private:
        sf::Time m_stepDuration, m_elapsed;
        std::function<void(const Step&, double)> m_callback;
    };
}
