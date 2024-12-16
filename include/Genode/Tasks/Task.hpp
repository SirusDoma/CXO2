#pragma once

#include <SFML/System/Time.hpp>

#include <Genode/Entities/Updatable.hpp>

#include <functional>

namespace Gx
{
    enum class TaskState
    {
        Idle,
        Running,
        Stopped,
        Completed
    };

    class Task : public Updatable
    {
    public:
        [[nodiscard]] TaskState GetState() const;
        [[nodiscard]] virtual const sf::Time& GetElapsed() const;

        void SetStartCallback(const std::function<void()>& callback);
        void SetStoppedCallback(const std::function<void()>& callback);
        void SetCompletedCallback(const std::function<void()>& callback);

        virtual void Stop();
        virtual void Complete();
        virtual void Reset();

        void Update(double delta) override;

    protected:
        Task();

        virtual void Initialize();
        void SetState(const TaskState& state);

    private:
        TaskState m_state;
        sf::Time m_elapsed;

        std::function<void()> m_startCallback, m_stopCallback, m_completeCallback;
    };
}
