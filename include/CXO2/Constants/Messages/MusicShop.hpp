#pragma once

#include <array>

namespace Cx::Constants::Messages::MusicShop
{
    constexpr static auto ALREADY_OWNED           = "Already server has the music, you don't have to buy"; // Unmapped
    constexpr static auto DOWNLOAD_CANCEL_CONFIRM = "A tune is now being downloaded. stop download files?"; // Unmapped

    namespace Purchase
    {
        constexpr static auto SUMMARY  = U"{}{}{}{} {}{}"; // Unmapped
        constexpr static auto TITLE    = "\nTitle : "; // Unmapped
        constexpr static auto PRICE    = "\n\nPrice : "; // Unmapped
        constexpr static auto CURRENCY = "Gem."; // Unmapped
        constexpr static auto CONFIRM  = "\n\nDo you want to buy the selected tune?"; // Unmapped
    }

    namespace MusicList
    {
        constexpr static auto LEVELS      = U"{}/{}/{}"; // Unmapped
        constexpr static auto DURATION    = U"{}:{:02}"; // Unmapped
    }

    namespace Download
    {
        constexpr static auto INITIALIZING     = "[Initialization]"; // Unmapped
        constexpr static auto CONNECTING       = "[Connecting to server]"; // Unmapped
        constexpr static auto DOWNLOADING      = "[Downloading]"; // Unmapped
        constexpr static auto RENAMING         = "[Changine a file name]"; // Unmapped
        constexpr static auto COMPLETED        = "[Downloading of a file is completed.]"; // Unmapped
        constexpr static auto CONNECT_FAILED   = "[Failed in connecting to the server]"; // Unmapped
        constexpr static auto DISK_FULL        = "[Shortage of disk capacity]"; // Unmapped
        constexpr static auto FILE_NOT_FOUND   = "[Cannot find the file specified]"; // Unmapped
        constexpr static auto FAILED_NOTICE    = "An error occurred while downloading. Please try it again after a little while later."; // Unmapped
        constexpr static auto DISK_FULL_NOTICE = "Shortage of disk capacity. Please check the capacity."; // Unmapped
        constexpr static auto CONNECT_FAILED_NOTICE = "Failed in connecting to the server to download a file. Please try it again after a little while later."; // Unmapped

        constexpr static auto DURATION         = U"({}:{:02})"; // Unmapped
        constexpr static auto PROGRESS_COUNT   = U"{}/{}"; // Unmapped
        constexpr static auto PROGRESS_ELAPSED = U"{:2}:{:02}:{:02}"; // Unmapped
        constexpr static auto PROGRESS_PERCENT = U"{:04.1f}"; // Unmapped
    }
}
