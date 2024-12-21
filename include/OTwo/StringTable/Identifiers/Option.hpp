#pragma once
#include <array>

namespace StringTable::Identifiers::Resource::Option
{
    constexpr static auto IDC_IMAGE_DIALOG_OPTION    = "IDC_IMAGE_DIALOG_OPTION";
    constexpr static auto IDC_TOOLTIP_INFO           = "IDC_TOOLTIP_INFO";
    constexpr static auto IDC_CONTAINER_GAME_OPTION  = "IDC_CONTAINER_GAME_OPTION";
    constexpr static auto IDC_CONTAINER_MUSIC_OPTION = "IDC_CONTAINER_MUSIC_OPTION";
    constexpr static auto IDC_IMAGE_KEY_SELECT       = "IDC_IMAGE_KEY_SELECT";
    constexpr static auto IDC_IMAGE_KEY_BAR          = "IDC_IMAGE_KEY_BAR";
    constexpr static auto IDC_TOGGLE_GFX             = "IDC_TOGGLE_GFX";
    constexpr static auto IDC_TOGGLE_CURSOR          = "IDC_TOGGLE_CURSOR";
    constexpr static auto IDC_TOGGLE_KEY_TEST        = "IDC_TOGGLE_KEY_TEST";
    constexpr static auto IDC_TOGGLE_BGM             = "IDC_TOGGLE_BGM";
    constexpr static auto IDC_GAUGE_MASTER_VOLUME    = "IDC_GAUGE_MASTER_VOLUME";
    constexpr static auto IDC_GAUGE_MUSIC_VOLUME     = "IDC_GAUGE_MUSIC_VOLUME";
    constexpr static auto IDC_GAUGE_SOUND_VOLUME     = "IDC_GAUGE_SOUND_VOLUME";
    constexpr static auto IDC_BUTTON_MASTER_UP       = "IDC_BUTTON_MASTER_UP";
    constexpr static auto IDC_BUTTON_MASTER_DOWN     = "IDC_BUTTON_MASTER_DOWN";
    constexpr static auto IDC_BUTTON_MUSIC_UP        = "IDC_BUTTON_MUSIC_UP";
    constexpr static auto IDC_BUTTON_MUSIC_DOWN      = "IDC_BUTTON_MUSIC_DOWN";
    constexpr static auto IDC_BUTTON_SOUND_UP        = "IDC_BUTTON_SOUND_UP";
    constexpr static auto IDC_BUTTON_SOUND_DOWN      = "IDC_BUTTON_SOUND_DOWN";
    constexpr static auto IDC_BUTTON_KEY_TAB         = "IDC_BUTTON_KEY_TAB";
    constexpr static auto IDC_BUTTON_DEFAULT         = "IDC_BUTTON_DEFAULT";
    constexpr static auto IDC_BUTTON_SOUND_TAB       = "IDC_BUTTON_SOUND_TAB";
    constexpr static auto IDC_BUTTON_SAVE            = "IDC_BUTTON_SAVE";

    constexpr static std::array<const char*, 7> IDC_IMAGE_KEY_TEXT =
    {
        "IDC_IMAGE_KEY_TEXT_1",
        "IDC_IMAGE_KEY_TEXT_2",
        "IDC_IMAGE_KEY_TEXT_3",
        "IDC_IMAGE_KEY_TEXT_4",
        "IDC_IMAGE_KEY_TEXT_5",
        "IDC_IMAGE_KEY_TEXT_6",
        "IDC_IMAGE_KEY_TEXT_7",
    };

    constexpr static std::array<const char*, 7> IDC_IMAGE_KEY_DOWN =
    {
        "IDC_IMAGE_KEY_DOWN_1",
        "IDC_IMAGE_KEY_DOWN_2",
        "IDC_IMAGE_KEY_DOWN_3",
        "IDC_IMAGE_KEY_DOWN_4",
        "IDC_IMAGE_KEY_DOWN_5",
        "IDC_IMAGE_KEY_DOWN_6",
        "IDC_IMAGE_KEY_DOWN_7",
    };
}
