#pragma once

#include <fmt/format.h>

namespace StringTable::Identifiers::Resource::Game
{
    namespace Renderer
    {
        constexpr static auto IDC_NOTE_CONTAINER      = "IDC_NOTE_CONTAINER";
        constexpr static auto IDC_IMAGE_NOTE_MEASURE1 = "IDC_IMAGE_NOTE_MEASURE1";
        constexpr static auto IDC_IMAGE_NOTE_MEASURE2 = "IDC_IMAGE_NOTE_MEASURE2";

        static std::string IDC_MEASURE(const std::size_t n)
        {
            return fmt::format("IDC_MEASURE_{}", n);
        }

        static std::string IDC_ANIMATION_NOTE_NORMAL(const std::size_t i, const std::size_t n)
        {
            return fmt::format("IDC_ANIMATION_NOTE_NORMAL{}_{}", i, n);
        }

        static std::string IDC_ANIMATION_NOTE_LONG(const std::size_t i, const std::size_t n)
        {
            return fmt::format("IDC_ANIMATION_NOTE_LONG{}_{}", i, n);
        }

        static std::string IDC_TAP_NOTE(const std::size_t n)
        {
            return fmt::format("IDC_TAP_NOTE_{}", n);
        }

        static std::string IDC_LONG_NOTE(const std::size_t n)
        {
            return fmt::format("IDC_LONG_NOTE_{}", n);
        }
    }

    namespace Menu
    {
        constexpr static auto IDC_CHART_RENDERER          = "IDC_CHART_RENDERER";
        constexpr static auto IDC_EQUALIZER               = "IDC_EQUALIZER";
        constexpr static auto IDC_TEXT_MUSIC_TITLE        = "IDC_TEXT_MUSIC_TITLE";
        constexpr static auto IDC_ANIMATION_PLAY_ICON     = "IDC_ANIMATION_PLAY_ICON";
        constexpr static auto IDC_IMAGE_MUSIC_LEVEL       = "IDC_IMAGE_MUSIC_LEVEL";
        constexpr static auto IDC_GAUGE_WAVE              = "IDC_GAUGE_WAVE";
        constexpr static auto IDC_NUMBER_PLAY_TIME_MINUTE = "IDC_NUMBER_PLAY_TIME_MINUTE";
        constexpr static auto IDC_NUMBER_PLAY_TIME_SECOND = "IDC_NUMBER_PLAY_TIME_SECOND";
        constexpr static auto IDC_GAUGE_VOLUME_MUSIC      = "IDC_GAUGE_VOLUME_MUSIC";
        constexpr static auto IDC_GAUGE_VOLUME_EFFECT     = "IDC_GAUGE_VOLUME_EFFECT";
        constexpr static auto IDC_LIST_NOTE_POINT_NUMBER  = "IDC_LIST_NOTE_POINT_NUMBER";
    }

    namespace Equalizer
    {
        constexpr static auto IDC_LIST_BAR = "IDC_LIST_BAR";
    }
}