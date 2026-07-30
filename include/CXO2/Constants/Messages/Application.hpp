#pragma once

namespace Cx::Constants::Messages::Application
{
    constexpr static auto WINDOW_TITLE = "O2-JAM";

    namespace Display
    {
        constexpr static auto DEVICE_3D_UNSUPPORTED = "Not supporting 3D device (Please set up a video card drive)"; // Unused
        constexpr static auto ENABLE_3D_CONFIRM     = "Do you use 3D effect? (If the effect does not appear properly, please press 'F7')";
        constexpr static auto DISABLE_3D_CONFIRM    = "Change to 2D effect? (If you want 3D effect, please press 'F7'.)";
        constexpr static auto IMAGE_CURSOR_ENABLED  = "Have changed to Image cursor. (If you want window cursor, press 'F8')";
        constexpr static auto WINDOW_CURSOR_CONFIRM = "Change to Window Cursor? (If no cursor is viewed, press 'F8')";
    }

    namespace Exit
    {
        constexpr static auto CONFIRM                   = "Would you like to finish the game?";
        constexpr static auto CONFIRM_WHILE_DOWNLOADING = "Would you like to stop download music files and then finish the game?";
    }
}
