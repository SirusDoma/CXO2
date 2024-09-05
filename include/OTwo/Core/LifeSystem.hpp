#ifndef O2JAM_CORE_LIFE_SYSTEM_HPP
#define O2JAM_CORE_LIFE_SYSTEM_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Models/Game.hpp>

class ScoreTracker;
class LifeSystem
{
public:
    static constexpr unsigned int DefaultMaxLifePoint = 1000;

    LifeSystem() = default;
    explicit LifeSystem(Difficulty diff);

    void Initialize(Difficulty diff = Difficulty::MX, int maxLifePoint = DefaultMaxLifePoint);

    void Update(Accuracy acc, unsigned int count = 1);

    Difficulty GetDifficulty() const;
    unsigned int GetMaxLifePoint() const;
    unsigned int GetCurrentLifePoint() const;

    void Reset();

private:
    Difficulty m_difficulty;

    int m_max;
    int m_point;
};


#endif
