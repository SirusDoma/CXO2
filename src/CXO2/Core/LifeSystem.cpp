#include <CXO2/Core/LifeSystem.hpp>
#include <Genode/System/Exception.hpp>

namespace Cx
{
    LifeSystem::LifeSystem(Difficulty diff) :
        m_difficulty(Difficulty::MX),
        m_max(DefaultMaxLifePoint),
        m_point(0)
    {
    }

    void LifeSystem::Initialize(const Difficulty diff, const int maxLifePoint)
    {
        if (static_cast<int>(maxLifePoint) < 0)
            throw Gx::Exception("Max life point must be a positive integer");

        m_difficulty = diff;
        m_max = maxLifePoint;
        Reset();
    }

    void LifeSystem::Update(const Accuracy acc, const unsigned int count)
    {
        switch (m_difficulty)
        {
            case Difficulty::EX:
            {
                switch (acc)
                {
                    case Accuracy::Cool: m_point += 3  * count; break;
                    case Accuracy::Good: m_point += 2  * count; break;
                    case Accuracy::Bad:  m_point -= 10 * count; break;
                    case Accuracy::Miss: m_point -= 50 * count; break;
                    default: break;
                }
            }
            break;
            case Difficulty::NX:
            {
                switch (acc)
                {
                    case Accuracy::Cool: m_point += 2  * count; break;
                    case Accuracy::Good: m_point += 1  * count; break;
                    case Accuracy::Bad:  m_point -= 7  * count; break;
                    case Accuracy::Miss: m_point -= 40 * count; break;
                    default: break;
                }
            }
            break;
            case Difficulty::HX:
            {
                switch (acc)
                {
                    case Accuracy::Cool: m_point += 1  * count; break;
                    case Accuracy::Bad:  m_point -= 5  * count; break;
                    case Accuracy::Miss: m_point -= 30 * count; break;
                    default: break;
                }
            }
            break;
            default: break;
        }

        m_point = std::min(m_point, m_max);
        m_point = std::max(m_point, 0);
    }

    Difficulty LifeSystem::GetDifficulty() const
    {
        return m_difficulty;
    }

    unsigned int LifeSystem::GetMaxLifePoint() const
    {
        return m_max;
    }

    unsigned int LifeSystem::GetCurrentLifePoint() const
    {
        return m_point;
    }

    void LifeSystem::Reset()
    {
        m_point = m_max;
    }
}
