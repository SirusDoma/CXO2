#pragma once

#include <CXO2/Metadata/UI/RadioButtonMetadata.hpp>
#include <CXO2/Models/Game.hpp>

namespace Cx
{
    struct SpeedButtonMetadata : public RadioButtonMetadata
    {
        float Speed = 0.f;
        SpeedMode Mode = SpeedMode::HiSpeed;
    };
}
