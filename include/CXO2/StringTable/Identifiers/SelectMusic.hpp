#pragma once

#include <fmt/format.h>

namespace Cx::StringTable::Identifiers::Resource::SelectMusic
{
    constexpr static auto IDC_DIALOG_SELECT_MUSIC           = "*/IDC_DIALOG_SELECT_MUSIC";
    constexpr static auto IDC_LIST_MUSIC_SELECTOR           = "IDC_LIST_MUSIC_SELECTOR";
    constexpr static auto IDC_IMAGE_MUSIC_ACTIVE            = "IDC_IMAGE_MUSIC_ACTIVE";
    constexpr static auto IDC_IMAGE_MUSIC_HIGHLIGHT         = "IDC_IMAGE_MUSIC_HIGHLIGHT";
    constexpr static auto IDC_BUTTON_LEFT                   = "IDC_BUTTON_LEFT";
    constexpr static auto IDC_BUTTON_RIGHT                  = "IDC_BUTTON_RIGHT";
    constexpr static auto IDC_CONTAINER_SORT_SELECTOR       = "IDC_CONTAINER_SORT_SELECTOR";
    constexpr static auto IDC_CONTAINER_GENRE_SELECTOR      = "IDC_CONTAINER_GENRE_SELECTOR";
    constexpr static auto IDC_CONTAINER_RANDOM_SELECTOR     = "IDC_CONTAINER_RANDOM_SELECTOR";
    constexpr static auto IDC_CONTAINER_DIFFICULTY_SELECTOR = "IDC_CONTAINER_DIFFICULTY_SELECTOR";
    constexpr static auto IDC_CONTAINER_SPEED_SELECTOR      = "IDC_CONTAINER_SPEED_SELECTOR";
    constexpr static auto IDC_TEXT_MUSIC_PAGE               = "IDC_TEXT_MUSIC_PAGE";
    constexpr static auto IDC_TEXT_MUSIC_TITLE              = "IDC_TEXT_MUSIC_TITLE";
    constexpr static auto IDC_TEXT_MUSIC_LEVEL              = "IDC_TEXT_MUSIC_LEVEL";
    constexpr static auto IDC_TEXT_MUSIC_TIME               = "IDC_TEXT_MUSIC_TIME";
    constexpr static auto IDC_LIST_MUSIC_INFO               = "IDC_LIST_MUSIC_INFO";
    constexpr static auto IDC_IMAGE_MUSIC_THUMBNAIL         = "IDC_IMAGE_MUSIC_THUMBNAIL";

    template<typename T>
    static std::string IDC_RADIO_SPEED(T&& speed)
    {
        return fmt::format("IDC_RADIO_SPEED_{}", speed);
    }

    namespace Sort
    {
        constexpr static auto IDC_BUTTON_SORT_NEW   = "IDC_BUTTON_SORT_NEW";
        constexpr static auto IDC_BUTTON_SORT_TITLE = "IDC_BUTTON_SORT_TITLE";
        constexpr static auto IDC_BUTTON_SORT_DIFF  = "IDC_BUTTON_SORT_DIFF";
        constexpr static auto IDC_BUTTON_SORT_TIME  = "IDC_BUTTON_SORT_TIME";
    }

    namespace Difficulty
    {
        constexpr static auto IDC_RADIO_NOTE_EX = "IDC_RADIO_NOTE_EX";
        constexpr static auto IDC_RADIO_NOTE_NX = "IDC_RADIO_NOTE_NX";
        constexpr static auto IDC_RADIO_NOTE_HX = "IDC_RADIO_NOTE_HX";
    }

    namespace Genre
    {
        constexpr static auto IDC_RADIO_GENRE_ALL         = "IDC_RADIO_GENRE_ALL";
        constexpr static auto IDC_RADIO_GENRE_BALLAD      = "IDC_RADIO_GENRE_BALLAD";
        constexpr static auto IDC_RADIO_GENRE_ROCK        = "IDC_RADIO_GENRE_ROCK";
        constexpr static auto IDC_RADIO_GENRE_DANCE       = "IDC_RADIO_GENRE_DANCE";
        constexpr static auto IDC_RADIO_GENRE_TECHNO      = "IDC_RADIO_GENRE_TECHNO";
        constexpr static auto IDC_RADIO_GENRE_HIPHOP      = "IDC_RADIO_GENRE_HIPHOP";
        constexpr static auto IDC_RADIO_GENRE_SOUL        = "IDC_RADIO_GENRE_SOUL";
        constexpr static auto IDC_RADIO_GENRE_JAZZ        = "IDC_RADIO_GENRE_JAZZ";
        constexpr static auto IDC_RADIO_GENRE_FUNK        = "IDC_RADIO_GENRE_FUNK";
        constexpr static auto IDC_RADIO_GENRE_CLASSICAL   = "IDC_RADIO_GENRE_CLASSICAL";
        constexpr static auto IDC_RADIO_GENRE_TRADITIONAL = "IDC_RADIO_GENRE_TRADITIONAL";
        constexpr static auto IDC_RADIO_GENRE_ETC         = "IDC_RADIO_GENRE_ETC";
    }

    namespace Random
    {
        constexpr static auto IDC_TOGGLE_RANDOM_DIFF_1 = "IDC_TOGGLE_RANDOM_DIFF_1";
        constexpr static auto IDC_TOGGLE_RANDOM_DIFF_2 = "IDC_TOGGLE_RANDOM_DIFF_2";
        constexpr static auto IDC_TOGGLE_RANDOM_DIFF_3 = "IDC_TOGGLE_RANDOM_DIFF_3";
        constexpr static auto IDC_TOGGLE_RANDOM_DIFF_4 = "IDC_TOGGLE_RANDOM_DIFF_4";
    }
}
