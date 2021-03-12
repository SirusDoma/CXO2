#ifndef GENODE_TASK_DELAY_HPP
#define GENODE_TASK_DELAY_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Delay : public Task
    {
    public:
        Delay();
        Delay(double duration);
        virtual ~Delay();

        double GetDuration() const;
        void SetDuration(double duration);

    protected:
        virtual void Update(double delta);

    private:
        double m_duration;
    };
}

#endif