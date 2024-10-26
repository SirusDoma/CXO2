#pragma once

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class Delay : public Task
    {
    public:
        Delay();
        explicit Delay(sf::Time duration);
        Delay(sf::Time duration, const std::function<void()>& callback);

        sf::Time GetDuration() const;
        void SetDuration(const sf::Time& duration);

    protected:
        void Update(double delta) override;

    private:
        sf::Time m_duration;
    };
}
