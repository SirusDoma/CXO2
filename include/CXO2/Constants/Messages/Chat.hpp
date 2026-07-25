#pragma once

namespace Cx::Constants::Messages::Chat
{
    namespace Commands
    {
        constexpr static auto HELP       = "/help";
        constexpr static auto HELP_SHORT = "/h";
        constexpr static auto WHISPER    = "/w ";
        constexpr static auto ROOM_TITLE = "/r ";
        constexpr static auto EMOTICON   = "/e ";
        constexpr static auto ANNOUNCE   = "/n ";
    }

    namespace Emoticons
    {
        constexpr static auto PREFIX = "/";

        constexpr static auto BY_DEGREES        = "...";
        constexpr static auto OHM_LOSE          = "\xC0\xB9";
        constexpr static auto OHM_VICTORY       = "\xC5\xA9\xC5\xA9";
        constexpr static auto OHM_HAHA          = "\xC7\xCF\xC7\xCF";
        constexpr static auto OHM_SMILE         = "\xC8\xE5\xC8\xE5";
        constexpr static auto OBJECTION         = "\xBD\xC8\xBE\xEE";
        constexpr static auto APPROVAL          = "\xC1\xC1\xBE\xC6";
        constexpr static auto READY             = "\xB7\xB9\xB5\xF0";
        constexpr static auto GO                = "\xB0\xED";
        constexpr static auto HI                = "\xBE\xC8\xB3\xE7";
        constexpr static auto FLOWER            = "\xB2\xC9";
        constexpr static auto CLOVER            = "7";
        constexpr static auto BROKEN_HEART      = "\xB9\xCC\xBF\xF6";
        constexpr static auto EXCLAMATION_MARK2 = "!!";
        constexpr static auto NOTE              = "~";
        constexpr static auto COUNT             = "321";
        constexpr static auto HEART             = "\xBB\xE7\xB6\xFB\xC7\xD8";
        constexpr static auto QUESTION          = "?";
        constexpr static auto EXCLAMATION_MARK  = "!";
        constexpr static auto IDEA              = "\xB9\xDD\xC2\xA6";
    }

    namespace Tooltips
    {
        constexpr static auto WHISPER = "Send whisper.";
        constexpr static auto GUILD   = "Send a message to Guild members.";
        constexpr static auto FRIEND  = "Send a message to a friend.";
        constexpr static auto ALL     = "Send a message to all.";
    }

    namespace Lines
    {
        constexpr static auto MESSAGE          = U"[{:>12}] {}";
        constexpr static auto WHISPER_SENT     = U"[{:>12}] << {}";
        constexpr static auto WHISPER_RECEIVED = U"[{:>12}] >> {}";
    }

    constexpr static auto WHISPER_TARGET_PROMPT = "Please enter a member ID \nand press OK to send a whisper message.";
    constexpr static auto WHISPER_NOT_DELIVERED = U"The message was not delivered to {}.";
    constexpr static auto EMOTION_HINT          = "Click the emotion button";
}
