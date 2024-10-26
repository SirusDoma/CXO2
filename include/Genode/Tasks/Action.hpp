#pragma once

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
