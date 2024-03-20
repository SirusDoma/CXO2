#ifndef GENODE_TASKS_TASK_DELAY_HPP
#define GENODE_TASKS_TASK_DELAY_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Delay : public Task
    {
    public:
        Delay();
        explicit Delay(sf::Time duration);
        ~Delay() override = default;

        sf::Time GetDuration() const;
        void SetDuration(const sf::Time& duration);

    protected:
        void Update(double delta) override;

    private:
        sf::Time m_duration;
    };
}

#endif