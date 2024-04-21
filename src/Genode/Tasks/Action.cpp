#include <Genode/Tasks/Action.hpp>

namespace Gx
{
    Action::Action(std::function<void()> callback) :
        m_callback(std::move(callback))
    {
    }

    void Action::Update(const double delta)
    {
        Task::Update(delta);

        if (GetState() == TaskState::Running)
        {
            if (m_callback)
                m_callback();

            Complete();
        }
    }
}
