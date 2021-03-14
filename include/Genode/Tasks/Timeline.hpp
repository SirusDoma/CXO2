#ifndef GENODE_TIMELINE_HPP
#define GENODE_TIMELINE_HPP

#include <Genode/Tasks/Task.hpp>
#include <vector>

namespace Gx
{
    class Timeline : public Task
    {
    public:
        struct KeyFrame
        {
            Task*    KeyTask;
            sf::Time Offset;

            KeyFrame(const sf::Time& offset, Gx::Task* task);
            ~KeyFrame();
        };

        Timeline();
        virtual ~Timeline();

        template<typename... Args>
        Timeline* Add(KeyFrame* first, Args... args);
        Timeline* Add(KeyFrame* frame);

        template<typename... Args>
        Timeline* Remove(KeyFrame* first, Args... args);
        Timeline* Remove(KeyFrame* frame);

        virtual void Reset();

    protected:
        virtual void Update(double delta);

    private:
        std::vector<const KeyFrame*> m_frames;
    };
}

#include <Genode/Tasks/Timeline.inl>
#endif