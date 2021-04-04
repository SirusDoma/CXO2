#ifndef GENODE_TASKS_ACTION_HPP
#define GENODE_TASKS_ACTION_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Action : public Task
    {
    public:
        Action(std::function<void()> callback);
        virtual ~Action();

    protected:
        virtual void Update(double delta);

    private:
        std::function<void()> m_callback;
    };
}

#endif