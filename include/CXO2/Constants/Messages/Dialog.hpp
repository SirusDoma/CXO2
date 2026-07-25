#pragma once

namespace Cx::Constants::Messages::Dialog
{
    namespace Info
    {
        constexpr static auto CAPTION = "[INFO]";

        constexpr static auto ALREADY_RUNNING       = "O2JAM is already running.";
        constexpr static auto RUN_LAUNCHER          = "Please try to run O2jam.exe.";
        constexpr static auto ENCRYPT_MODULE_FAILED = "EGameEncrypt.dll error"; // Unused
        constexpr static auto PATCH_COMPLETED       = "Patch completed. Please run the game again."; // Unused
    }

    namespace Notice
    {
        constexpr static auto CAPTION = "[NOTICE]"; // Unused

        constexpr static auto PATCH_COMPLETED = "Patch completed. Please run the game again."; // Unused
    }

    namespace Error
    {
        constexpr static auto CAPTION = "[ERROR]"; // Unmapped

        constexpr static auto SERVER_UNREACHABLE   = "Cannot connect to server Error."; // Unmapped
        constexpr static auto DIRECTX_REQUIRED     = "Install DirectX Ver8.1 or higher"; // Unused
        constexpr static auto SOUND_DRIVER_MISSING = "Cannot find the soundcard driver. Reinstall soundcard driver"; // Unused

        constexpr static auto PLAYING_ARCHIVE_MISSING   = "Cannot find file that is playing1.opi."; // Unmapped
        constexpr static auto INTERFACE_ARCHIVE_MISSING = "Cannot find file that is interface1.opi."; // Unmapped
        constexpr static auto AVATAR_ARCHIVE_MISSING    = "Cannot find file that is avatar.opa."; // Unmapped
    }

    namespace Caution
    {
        constexpr static auto CAPTION = "Caution"; // Unmapped

        constexpr static auto PARSE_FAILED = "File format wrong and can't loading"; // Unmapped
    }

    namespace Warning
    {
        constexpr static auto CAPTION = "Waring"; // Unmapped

        constexpr static auto ARCHIVE_INIT_FAILED  = "[ERROR]File init error(Pack)"; // Unmapped
        constexpr static auto ARCHIVE_OPEN_FAILED  = "[ERROR]File open error(Pack)"; // Unmapped
        constexpr static auto ARCHIVE_CLOSE_FAILED = "[ERROR]File close error(Pack)"; // Unmapped
        constexpr static auto SURFACE_OPEN_FAILED  = "[ERROR]File open error(Surface)"; // Unmapped
        constexpr static auto INPUT_OPEN_FAILED    = "[ERROR]File open error(Input)"; // Unmapped
    }
}
