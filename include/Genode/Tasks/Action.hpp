#ifndef GENODE_TASKS_ACTION_HPP
#define GENODE_TASKS_ACTION_HPP

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Action : public Task
    {
    public:
        explicit Action(std::function<void()> callback);
        ~Action() override = default;

    protected:
        void Update(double delta) override;

    private:
        std::function<void()> m_callback;
    };
}

#endif