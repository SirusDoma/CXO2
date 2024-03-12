#ifndef GENODE_TASKS_SEQUENCE_HPP
#define GENODE_TASKS_SEQUENCE_HPP

#include <Genode/Tasks/Task.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <vector>

namespace Gx
{
    class Sequence : public Task
    {
    public:
        Sequence();
        Sequence(std::initializer_list<Task*> tasks);
        //Sequence(std::function<void()> callback);
        Sequence(std::function<void()> callback, std::initializer_list<Task*> tasks);
        virtual ~Sequence();

        template<typename... Args>
        Sequence* Add(Task* first, Args... args);
        Sequence* Add(Task* task);

        template<typename... Args>
        Sequence* Remove(Task* first, Args... args);
        Sequence* Remove(Task* task);

        //template<typename T>
        //static Sequence* Routine(T* task, unsigned int count, sf::Time delay = sf::Time::Zero);
        static std::initializer_list<Task*> ListOf(std::initializer_list<Task*> &&tasks);

        virtual void Reset();

    protected:
        virtual void Update(double delta);
        virtual void Complete();

    private:
        std::vector<Task*>::iterator m_iterator;
        std::vector<Task*> m_tasks;
    };
}

#include <Genode/Tasks/Sequence.inl>
#endif