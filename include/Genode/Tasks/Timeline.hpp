#pragma once

#include <Genode/Tasks/Task.hpp>
#include <vector>

namespace Gx
{
    class Timeline : public Task
    {
    public:
        struct KeyFrame
        {
            Task* Task;
            sf::Time Offset;

            KeyFrame(const sf::Time& offset, Gx::Task& task);
            bool operator==(const KeyFrame& rhs) const;
        };

        Timeline();

        template<typename... Args>
        Timeline* Add(const KeyFrame& first, const Args&... args);
        Timeline* Add(const KeyFrame& frame);

        template<typename... Args>
        Timeline* Remove(const KeyFrame& first, const Args&... args);
        Timeline* Remove(const KeyFrame& frame);

        void Reset() override;

    protected:
        void Update(double delta) override;

    private:
        std::vector<KeyFrame> m_frames;
    };
}

#include <Genode/Tasks/Timeline.inl>
