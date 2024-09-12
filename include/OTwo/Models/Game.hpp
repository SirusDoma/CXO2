#ifndef O2JAM_MODELS_GAME_HPP
#define O2JAM_MODELS_GAME_HPP

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <array>

constexpr std::array<float, 12> SupportedHiSpeeds = { 0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 4.5f, 5.f, 6.f, 8.f };

constexpr float XrSpeed = -1.0f;
constexpr float TdSpeed = -2.0f;

enum class GameMode : std::uint8_t
{
    Single,
    Versus,
    Album,
    Couple,
    Live,
    Jam
};

enum class  Difficulty : std::uint8_t
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

#endif
