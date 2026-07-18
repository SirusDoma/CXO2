#include <CXO2/UI/Waiting/SpeedButton.hpp>

namespace Cx
{
    float SpeedButton::GetSpeed() const
    {
        return m_speed;
    }

    void SpeedButton::SetSpeed(const float speed)
    {
        m_speed = speed;
    }

    SpeedMode SpeedButton::GetMode() const
    {
        return m_mode;
    }

    void SpeedButton::SetMode(const SpeedMode mode)
    {
        m_mode = mode;
    }
}
