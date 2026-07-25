#pragma once

namespace Cx::Constants::Messages::SelectMusic
{
    namespace MusicList
    {
        constexpr static auto NO_MUSIC_FOUND    = "no music found";
        constexpr static auto RANDOM_SELECTED   = U"'Random' is selected.(Total{}/{})";
        constexpr static auto PAGE              = U"[{:02}/{:02}]";
        constexpr static auto GENRE_UNAVAILABLE = U" {} not available yet.";
        constexpr static auto DURATION          = U"[{}:{:02}]";
    }

    namespace Fields
    {
        constexpr static auto TITLE  = U"Title";
        constexpr static auto ARTIST = U"Artist";
        constexpr static auto EDITOR = U"Editor";
        constexpr static auto NOTE   = U"Note";
        constexpr static auto BPM    = U"BPM";

        constexpr static auto TEXT_VALUE    = U" {} : {}";
        constexpr static auto INTEGER_VALUE = U" {} : {}";
        constexpr static auto DECIMAL_VALUE = U" {} : {:.2f}";
    }

    namespace RandomRange
    {
        constexpr static auto RANDOM_ITEM  = U"     LEVEL {}    ";
        constexpr static auto RANDOM_FIELD = U" LEVEL {}";

        constexpr static auto BEGINNER     = U" 1 -  5";
        constexpr static auto INTERMEDIATE = U" 5 -  9";
        constexpr static auto HIGH         = U" 9 - 13";
        constexpr static auto MASTER       = U" 13 higher";
    }

    namespace ToolTips
    {
        constexpr static auto BEGINNER     = "Level 1 - 5 tunes Random selection";
        constexpr static auto INTERMEDIATE = "Level 5 - 9 tunes Random selection";
        constexpr static auto HIGH         = "Level 9 - 13 tunes Random selection";
        constexpr static auto MASTER       = "Higher than level 13 tunes Random selection";
    }
}
