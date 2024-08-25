#ifndef GENODE_TASKS_TASK_STEP_HPP
#define GENODE_TASKS_TASK_STEP_HPP

#include <Genode/Tasks/Delay.hpp>

namespace Gx
{
    class Step : public Delay
    {
    public:
        Step();
        explicit Step(sf::Time duration, sf::Time stepDuration, const std::function<void(const Step&, double)> &update);
        ~Step() override = default;

    protected:
        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    private:
        sf::Time m_stepDuration, m_elapsed;
        std::function<void(const Step&, double)> m_callback;
    };
}

#endif