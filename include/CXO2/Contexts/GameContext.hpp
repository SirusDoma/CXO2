#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Models/Game.hpp>

#include <CXO2/Network/Events/GameCompletedEventData.hpp>

#include <array>
#include <cstdint>

namespace Cx
{
    struct GameContext
    {
        Gx::ResourcePtr<Cx::Chart> Chart;
        Cx::Difficulty             Difficulty{};
        GameMode                   Mode{};
        float                      Speed{};
        Cx::SpeedMode              SpeedMode{Cx::SpeedMode::HiSpeed};
        std::uint8_t               MapID{};
        std::uint8_t               EffectID{};

        std::array<GameCompletedEventData::ScoreEntry, 8> Scores{};
    };
}
