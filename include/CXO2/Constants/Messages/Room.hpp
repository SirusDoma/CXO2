#pragma once

namespace Cx::Constants::Messages::Room
{
    constexpr static auto NO_ROOM_AVAILABLE      = "No avaliable room.";
    constexpr static auto NO_TUNE_OWNED          = "You have no tune";
    constexpr static auto TITLE_CHANGE_FORBIDDEN = "Here you cannot change room title ";
    constexpr static auto USER_LIST_COUNT        = U"Users : {}({}/{})";
    constexpr static auto USER_LIST_ENTRY        = U"Lv.{}:{}";
    constexpr static auto SYNC_FAILED            = "[ERROR]can't receive from server character infomation";

    constexpr static auto LEVEL_RANGE = U" ({} ~ {})";
    constexpr static auto USER_COUNT  = U" ({}/{})";

    namespace Random
    {
        constexpr static auto RANGE = U"<< Random {} - {} >>";
        constexpr static auto ABOVE = U"<< Random {} or higher >>";
    }

    namespace JoinRequest
    {
        constexpr static auto TUNE_UNREGISTERED   = "This is the room with unregistered tune."; // Unused
        constexpr static auto SINGLE_MODE_CLOSED  = "As it is a single room, any user cannot enter.";
        constexpr static auto PASSWORD_PROMPT     = "Please input room password";
        constexpr static auto LEVEL_OUT_OF_RANGE  = U"[INFO]Wrong level is selected.\nYou can enter the level range from \n{} - {}";
        constexpr static auto TUNE_REQUIRED       = U"Please download the following tune. \n\n<< title : {} >>";
        constexpr static auto ROOM_FILLED         = "The room is filled.";
        constexpr static auto GAME_IN_PROGRESS    = "The game is in process.";

        constexpr static auto MUSIC_CORRUPTED      = "There may be a damage caused to the file. The tune is not registered."; // Unmapped
        constexpr static auto MUSIC_FORMAT_INVALID = U"File format wrong.\n{} Please delete files and download again"; // Unmapped
        constexpr static auto MUSIC_NOT_FOUND      = U"{} cannot find file"; // Unmapped
    }

    namespace JoinResponse
    {
        constexpr static auto ROOM_FULL      = "The room is full.";
        constexpr static auto GAME_STARTED   = "The game already has started.";
        constexpr static auto WRONG_PASSWORD = "Wrong password";
        constexpr static auto SINGLE_MODE    = "The room is single playmode";
        constexpr static auto UNKNOWN        = "[Fail] Please contact administrator when this message shows.";
    }

    namespace ChatHelp
    {
        constexpr static auto CURSOR_MODE = "F8          : cursor mode";
        constexpr static auto EFFECT_MODE = "F7          : 2D/3D effect mode";
        constexpr static auto WHISPER     = "/w Receiver : Send whisper";
    }

    namespace Welcome
    {
        constexpr static auto CURSOR_MODE = "F8          : Cursor mode setting";
        constexpr static auto EFFECT_MODE = "F7          : Effect 2D/3D mode setting";
        constexpr static auto WHISPER     = "/w Receiver : Send message(whisper)";
    }

    namespace CreateRoom
    {
        constexpr static auto DEFAULT_TITLE_SUFFIX  = U"'s room";
        constexpr static auto TITLE_REQUIRED        = "Please enter a room name.";
        constexpr static auto JAM_MODE_UNAVAILABLE  = "JAM mode is not available yet.";
        constexpr static auto LEVEL_OUT_OF_RANGE    = "Wrong level is selected. You can enter the level range from 1 to 100";
        constexpr static auto LEVEL_LIMIT_IN_SINGLE = "You can not set the level in single mode.";
        constexpr static auto LEVEL_LIMIT_REQUIRED  = "Please set the level limit. Ex) 10 ~ 20";
        constexpr static auto HIGH_LEVEL_TOO_LOW    = "You must enter higher numbers in HIGH LEVEL";
    }
}
