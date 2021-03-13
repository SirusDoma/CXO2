#include <Genode/Tasks/Action.hpp>

namespace Gx
{
    Action::Action(std::function<void()> callback)
    {
        OnCompleted(callback);
    }

    Action::~Action()
    {
    }

    void Action::Update(double delta)
    {
        Task::Update(delta);

        if (GetState() == Task::Running)
            Complete();
    }
}