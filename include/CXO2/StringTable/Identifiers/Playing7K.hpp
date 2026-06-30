#pragma once

#include <array>

namespace Cx::StringTable::Identifiers::Resource::Playing7K
{
    constexpr static auto IDC_CHART_RENDERER           = "STATE_PLAYING_7K/IDC_CHART_RENDERER";
    constexpr static auto IDC_CHAT_PANEL               = "STATE_PLAYING_7K/IDC_CHAT_PANEL";
    constexpr static auto IDC_EDIT_CHAT                = "STATE_PLAYING_7K/IDC_CHAT_PANEL/IDC_EDIT_CHAT";
    constexpr static auto IDC_ANIMATION_INSTRUCTOR     = "STATE_PLAYING_7K/IDC_ANIMATION_INSTRUCTOR";
    constexpr static auto IDC_IMAGE_INSTRUCTION        = "STATE_PLAYING_7K/IDC_IMAGE_INSTRUCTION";
    constexpr static auto IDC_LIST_AVATAR              = "STATE_PLAYING_7K/IDC_LIST_AVATAR";
    constexpr static auto IDC_CONTAINER_KEY_EFFECT     = "STATE_PLAYING_7K/IDC_CONTAINER_KEY_EFFECT";
    constexpr static auto IDC_CONTAINER_KEY_DOWN       = "STATE_PLAYING_7K/IDC_CONTAINER_KEY_DOWN";
    constexpr static auto IDC_PLAY_MENU                = "STATE_PLAYING_7K/IDC_PLAY_MENU";
    constexpr static auto IDC_NUMBER_POINT_NUMBER      = "STATE_PLAYING_7K/IDC_NUMBER_POINT_NUMBER";
    constexpr static auto IDC_GAUGE_JAM_BAR            = "STATE_PLAYING_7K/IDC_GAUGE_JAM_BAR";
    constexpr static auto IDC_CONTAINER_BUFFER         = "STATE_PLAYING_7K/IDC_CONTAINER_BUFFER";
    constexpr static auto IDC_GAUGE_LIFE_BAR           = "STATE_PLAYING_7K/IDC_GAUGE_LIFE_BAR";
    constexpr static auto IDC_CONTAINER_NOTE_JAM       = "STATE_PLAYING_7K/IDC_CONTAINER_NOTE_JAM";
    constexpr static auto IDC_ANIMATION_NOTE_JAM       = "STATE_PLAYING_7K/IDC_CONTAINER_NOTE_JAM/IDC_ANIMATION_NOTE_JAM";
    constexpr static auto IDC_NUMBER_NOTE_JAM          = "STATE_PLAYING_7K/IDC_CONTAINER_NOTE_JAM/IDC_NUMBER_NOTE_JAM";
    constexpr static auto IDC_CONTAINER_COMBO          = "STATE_PLAYING_7K/IDC_CONTAINER_COMBO";
    constexpr static auto IDC_ANIMATION_NOTE_COMBO     = "STATE_PLAYING_7K/IDC_ANIMATION_NOTE_COMBO";
    constexpr static auto IDC_NUMBER_NOTE_COMBO        = "STATE_PLAYING_7K/IDC_NUMBER_NOTE_COMBO";
    constexpr static auto IDC_ANIMATION_NOTE_COOL      = "STATE_PLAYING_7K/IDC_ANIMATION_NOTE_COOL";
    constexpr static auto IDC_ANIMATION_NOTE_GOOD      = "STATE_PLAYING_7K/IDC_ANIMATION_NOTE_GOOD";
    constexpr static auto IDC_ANIMATION_NOTE_BAD       = "STATE_PLAYING_7K/IDC_ANIMATION_NOTE_BAD";
    constexpr static auto IDC_ANIMATION_NOTE_MISS      = "STATE_PLAYING_7K/IDC_ANIMATION_NOTE_MISS";
    constexpr static auto IDC_NOTE_JUDGEMENT_INDICATOR = "STATE_PLAYING_7K/IDC_NOTE_JUDGEMENT_INDICATOR";
    constexpr static auto IDC_LIST_LONG_NOTE_EFFECT    = "STATE_PLAYING_7K/IDC_LIST_LONG_NOTE_EFFECT";
    constexpr static auto IDC_LIST_NOTE_CLICK          = "STATE_PLAYING_7K/IDC_LIST_NOTE_CLICK";
    constexpr static auto IDC_BUTTON_EXIT              = "STATE_PLAYING_7K/IDC_BUTTON_EXIT";

    namespace Require
    {
        constexpr static auto IDC_IMAGE_PLAYING_BG       = "IDC_IMAGE_PLAYING_BG";
        constexpr static auto IDC_IMAGE_NOTE_BG          = "IDC_IMAGE_NOTE_BG";
        constexpr static auto IDC_LIST_NOTE_CLICK        = "IDC_LIST_NOTE_CLICK";
        constexpr static auto IDC_LIST_LONG_NOTE_EFFECT  = "IDC_LIST_LONG_NOTE_EFFECT";
        constexpr static auto IDC_IMAGE_NOTE_MEASURE1    = "IDC_IMAGE_NOTE_MEASURE1";
        constexpr static auto IDC_IMAGE_NOTE_MEASURE2    = "IDC_IMAGE_NOTE_MEASURE2";
        constexpr static auto IDC_ANIMATION_NOTE_NORMAL  = "IDC_ANIMATION_NOTE_NORMAL";
        constexpr static auto IDC_ANIMATION_NOTE_LONG    = "IDC_ANIMATION_NOTE_LONG";
        constexpr static auto IDC_ANIMATION_NOTE_COMBO   = "IDC_ANIMATION_NOTE_COMBO";
        constexpr static auto IDC_NUMBER_NOTE_COMBO      = "IDC_NUMBER_NOTE_COMBO";
        constexpr static auto IDC_ANIMATION_EFFECT_JAM   = "IDC_ANIMATION_EFFECT_JAM";
        constexpr static auto IDC_ANIMATION_NOTE_COOL    = "IDC_ANIMATION_NOTE_COOL";
        constexpr static auto IDC_ANIMATION_NOTE_GOOD    = "IDC_ANIMATION_NOTE_GOOD";
        constexpr static auto IDC_ANIMATION_NOTE_BAD     = "IDC_ANIMATION_NOTE_BAD";
        constexpr static auto IDC_ANIMATION_NOTE_MISS    = "IDC_ANIMATION_NOTE_MISS";
        constexpr static auto IDC_NUMBER_EFFECT_JAM      = "IDC_NUMBER_EFFECT_JAM";
        constexpr static auto IDC_CONTAINER_NOTE_JAM     = "IDC_CONTAINER_NOTE_JAM";
        constexpr static auto IDC_CONTAINER_KEY_EFFECT   = "IDC_CONTAINER_KEY_EFFECT";
        constexpr static auto IDC_IMAGE_KEY_EFFECT       = "IDC_IMAGE_KEY_EFFECT";
        constexpr static auto IDC_IMAGE_GUIDE_KEY_EFFECT = "IDC_IMAGE_GUIDE_KEY_EFFECT";
        constexpr static auto IDC_ANIMATION_NOTE_CLICK   = "IDC_ANIMATION_NOTE_CLICK";
    }

    namespace Avatar
    {
        constexpr static auto IDC_AVATAR               = "IDC_AVATAR";
        constexpr static auto IDC_CONTAINER_EFFECT_JAM = "IDC_CONTAINER_EFFECT_JAM";
        constexpr static auto IDC_ANIMATION_EFFECT_JAM = "IDC_ANIMATION_EFFECT_JAM";
        constexpr static auto IDC_NUMBER_EFFECT_JAM    = "IDC_NUMBER_EFFECT_JAM";
    }

    constexpr static std::array<const char*, 7> IDC_IMAGE_KEY_DOWN =
    {
        "IDC_IMAGE_KEY_DOWN1",
        "IDC_IMAGE_KEY_DOWN2",
        "IDC_IMAGE_KEY_DOWN3",
        "IDC_IMAGE_KEY_DOWN4",
        "IDC_IMAGE_KEY_DOWN5",
        "IDC_IMAGE_KEY_DOWN6",
        "IDC_IMAGE_KEY_DOWN7",
    };

    constexpr static std::array<const char*, 7> IDC_IMAGE_KEY_EFFECT =
    {
        "IDC_IMAGE_KEY_EFFECT1",
        "IDC_IMAGE_KEY_EFFECT2",
        "IDC_IMAGE_KEY_EFFECT3",
        "IDC_IMAGE_KEY_EFFECT4",
        "IDC_IMAGE_KEY_EFFECT5",
        "IDC_IMAGE_KEY_EFFECT6",
        "IDC_IMAGE_KEY_EFFECT7",
    };

    constexpr static std::array<const char*, 7> IDC_ANIMATION_LONG_NOTE_EFFECT =
    {
        "IDC_ANIMATION_LONG_NOTE_EFFECT1",
        "IDC_ANIMATION_LONG_NOTE_EFFECT2",
        "IDC_ANIMATION_LONG_NOTE_EFFECT3",
        "IDC_ANIMATION_LONG_NOTE_EFFECT4",
        "IDC_ANIMATION_LONG_NOTE_EFFECT5",
        "IDC_ANIMATION_LONG_NOTE_EFFECT6",
        "IDC_ANIMATION_LONG_NOTE_EFFECT7",
    };

    constexpr static std::array<const char*, 7> IDC_ANIMATION_NOTE_CLICK =
    {
        "IDC_ANIMATION_NOTE_CLICK1",
        "IDC_ANIMATION_NOTE_CLICK2",
        "IDC_ANIMATION_NOTE_CLICK3",
        "IDC_ANIMATION_NOTE_CLICK4",
        "IDC_ANIMATION_NOTE_CLICK5",
        "IDC_ANIMATION_NOTE_CLICK6",
        "IDC_ANIMATION_NOTE_CLICK7",
    };

    constexpr static std::array<const char*, 7> IDC_IMAGE_GUIDE_KEY_EFFECT =
    {
        "IDC_IMAGE_GUIDE_KEY_EFFECT1",
        "IDC_IMAGE_GUIDE_KEY_EFFECT2",
        "IDC_IMAGE_GUIDE_KEY_EFFECT3",
        "IDC_IMAGE_GUIDE_KEY_EFFECT4",
        "IDC_IMAGE_GUIDE_KEY_EFFECT5",
        "IDC_IMAGE_GUIDE_KEY_EFFECT6",
        "IDC_IMAGE_GUIDE_KEY_EFFECT7",
    };


}
