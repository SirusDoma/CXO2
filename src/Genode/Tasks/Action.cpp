#include <Genode/Tasks/Action.hpp>

namespace Gx
{
    Action::Action(std::function<void()> callback) :
        m_callback(callback)
    {
    }

    Action::~Action()
    {
    }

    void Action::Update(double delta)
    {
        Task::Update(delta);

        if (GetState() == Task::Running)
        {
            if (m_callback)
                m_callback();

            Complete();
        }
    }
}