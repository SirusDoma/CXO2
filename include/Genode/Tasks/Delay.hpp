#ifndef GENODE_TASKS_TASK_DELAY_HPP
#define GENODE_TASKS_TASK_DELAY_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Delay : public Task
    {
    public:
        Delay();
        Delay(sf::Time duration);
        virtual ~Delay();

        sf::Time GetDuration() const;
        void SetDuration(const sf::Time& duration);

    protected:
        virtual void Update(double delta);

    private:
        sf::Time m_duration;
    };
}

#endif