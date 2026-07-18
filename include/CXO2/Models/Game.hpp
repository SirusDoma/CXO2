#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace Cx
{
    constexpr std::array<float, 12> SupportedHiSpeeds = { 0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 4.5f, 5.f, 6.f, 8.f };

    constexpr std::array<float, 10> OfficialHiSpeeds  = { 1.f, 1.5f, 2.f, 2.5f, 3.f, 4.f, 5.f, 6.f, 8.f, 0.5f };

    enum class SpeedMode : std::uint8_t
    {
        HiSpeed = 0,
        XrSpeed = 1,
        TdSpeed = 2
    };

    enum class Speed : std::uint8_t
    {
        X10 = 0,
        X15 = 1,
        X20 = 2,
        X25 = 3,
        X30 = 4,
        X40 = 5,
        X50 = 6,
        X60 = 7,
        X80 = 8,
        X05 = 9
    };

    constexpr std::optional<float> ToSpeedValue(const Speed speed)
    {
        const auto index = static_cast<std::size_t>(speed);
        if (index >= OfficialHiSpeeds.size())
            return std::nullopt;

        return OfficialHiSpeeds[index];
    }

    constexpr std::optional<Speed> ToSpeed(const float value)
    {
        for (std::size_t i = 0; i < OfficialHiSpeeds.size(); i++)
        {
            if (OfficialHiSpeeds[i] == value)
                return static_cast<Speed>(i);
        }

        return std::nullopt;
    }

    constexpr SpeedMode ToSpeedMode(const Speed)
    {
        // In the future XR (or even TD) might baked into the Speed
        return SpeedMode::HiSpeed;
    }

    enum class GameMode : std::uint8_t
    {
        Single,
        Versus,
        Jam,
        Album,
        Couple,
        Live,
        Tutorial
    };

    enum class Difficulty : std::uint8_t
    {
        EX = 0,
        NX = 1,
        HX = 2,
        MX = 3
    };

    enum class Accuracy : std::uint8_t
    {
        None = 0,
        Cool = 1,
        Good = 2,
        Bad  = 3,
        Miss = 4
    };
}
