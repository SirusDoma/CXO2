#ifndef GENODE_TASKS_ACTION_HPP
#define GENODE_TASKS_ACTION_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Action : public Task
    {
    public:
        Action() = default;
        explicit Action(std::function<void()> callback);

    protected:
        void Update(double delta) override;

    private:
        std::function<void()> m_callback;
    };
}

#endif