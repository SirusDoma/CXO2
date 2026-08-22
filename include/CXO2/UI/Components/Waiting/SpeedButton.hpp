#pragma once

#include <CXO2/Models/Game.hpp>

#include <CXO2/UI/RadioButton.hpp>

namespace Cx
{
    class SpeedButton : public Cx::RadioButton
    {
    public:
        SpeedButton() = default;

        float GetSpeed() const;
        void SetSpeed(float speed);

        SpeedMode GetMode() const;
        void SetMode(SpeedMode mode);

    private:
        float     m_speed = 0.f;
        SpeedMode m_mode  = SpeedMode::HiSpeed;
    };
}
