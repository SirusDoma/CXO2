#pragma once

#include<fmt/format.h>
#include <string>

namespace StringTable::Identifiers::Resource::Map
{
    constexpr static auto IDC_CONTAINER_MAP_SELECTOR  = "*/IDC_CONTAINER_MAP_SELECTOR";

    constexpr static auto IDC_IMAGE_MAP               = "IDC_IMAGE_MAP";
    constexpr static auto IDC_IMAGE_RANDOM_MAP        = "IDC_IMAGE_RANDOM_MAP";
    constexpr static auto IDC_TEXT_MAP_NAME           = "IDC_TEXT_MAP_NAME";
    constexpr static auto IDC_CONTAINER_EFFECT_1      = "IDC_CONTAINER_EFFECT_1";
    constexpr static auto IDC_CONTAINER_EFFECT_2      = "IDC_CONTAINER_EFFECT_2";
    constexpr static auto IDC_IMAGE_EFFECT0_1         = "IDC_IMAGE_EFFECT0_1";
    constexpr static auto IDC_IMAGE_EFFECT0_2         = "IDC_IMAGE_EFFECT0_2";
    constexpr static auto IDC_BUTTON_MAP_LEFT         = "IDC_BUTTON_MAP_LEFT";
    constexpr static auto IDC_BUTTON_MAP_RIGHT        = "IDC_BUTTON_MAP_RIGHT";
    constexpr static auto IDC_IMAGE_COVER_LEFT        = "IDC_IMAGE_COVER_LEFT";
    constexpr static auto IDC_IMAGE_COVER_RIGHT       = "IDC_IMAGE_COVER_RIGHT";
    constexpr static auto IDC_RADIO_MAP_SELECT_TOP    = "IDC_RADIO_MAP_SELECT_TOP";
    constexpr static auto IDC_RADIO_MAP_SELECT_BOTTOM = "IDC_RADIO_MAP_SELECT_BOTTOM";

    static std::string IDC_IMAGE_EFFECT_I_1(const std::size_t i)
    {
        return fmt::format("IDC_IMAGE_EFFECT{}_1", i);
    };

    static std::string IDC_IMAGE_EFFECT_I_2(const std::size_t i)
    {
        return fmt::format("IDC_IMAGE_EFFECT{}_2", i);
    };
}
