#pragma once

namespace Cx::Constants::Messages::Waiting
{

    constexpr static auto TUNE_NOT_SELECTED       = "No tune to play. Please select a tune again.";
    constexpr static auto TUNE_NOT_FOUND          = "Failed in finding the tune. Please download it.";
    constexpr static auto TUNE_MISSING_FOR_OTHERS = "There are users who have not the right tune.";
    constexpr static auto TUNE_NOT_PURCHASED      = "You have a music file but did not purchase any. Please purchase some in the music shop.";

    namespace Music
    {
        constexpr static auto RANDOM_OVER = U"<< Random {} over >>";
    }

    namespace ChangeRoomTitle
    {
        constexpr static auto TITLE_PROMPT           = "Please enter a room name.";
        constexpr static auto TITLE_TOO_LONG         = "[INFO] Room title max length is 21";
        constexpr static auto TITLE_CHANGE_FORBIDDEN = "Room master only can change the room title";
    }

    namespace Members
    {
        constexpr static auto ENTRY        = U"Lv:{:2} {}";
        constexpr static auto JOINED       = U"[INFO] {} just came in.";
        constexpr static auto LEFT         = U"[INFO] {} has left";
        constexpr static auto FORCED_OUT   = U"[INFO] {} forced out.";
        constexpr static auto KICK_CONFIRM = "Would you like to kick out?";
        constexpr static auto KICKED       = "You are forced out";
    }

    namespace GameStart
    {
        constexpr static auto INSUFFICIENT_PLAYERS = "For VS mode, more than one player is required.";
        constexpr static auto TEAMS_UNBALANCED     = "The team is not in harmony.";
        constexpr static auto PLAYERS_NOT_READY    = "There are users not ready yet";
    }

    namespace ChatHelp
    {
        constexpr static auto EMOTION     = "(Ctrl + 0 ~ 9 key) or (Ctrl+ Shift + 0 ~ 9 key) : Display emotion";
        constexpr static auto CURSOR_MODE = "F8            : cursor mode";
        constexpr static auto EFFECT_MODE = "F7            : 2D/3D effect mode";
        constexpr static auto ROOM_TITLE  = "/r Room title : Change a room name";
        constexpr static auto WHISPER     = "/w Receiver   : Send whisper";
    }

    namespace Welcome
    {
        constexpr static auto CHANGE_TITLE = "/r Room title : Change room title";
    }
}
