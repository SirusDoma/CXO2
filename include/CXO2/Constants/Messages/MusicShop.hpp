#pragma once

#include <array>

namespace Cx::Constants::Messages::MusicShop
{
    constexpr static auto ALREADY_FREE           = "Already server has the music, you don't have to buy";
    constexpr static auto DOWNLOAD_CANCEL_CONFIRM = "A tune is now being downloaded.\nstop download files?";

    namespace Purchase
    {
        constexpr static auto PROMPT   = U"{}{}{}{} {}{}";
        constexpr static auto TITLE    = U"\nTitle : ";
        constexpr static auto PRICE    = U"\n\nPrice : ";
        constexpr static auto CURRENCY = U"Gem.";
        constexpr static auto CONFIRM  = U"\n\nDo you want to buy the selected tune?";
    }

    namespace ShopList
    {
        constexpr static auto LEVELS   = U"{}/{}/{}";
        constexpr static auto DURATION = U"({}:{:02})";
    }

    namespace MusicList
    {
        constexpr static auto LEVELS      = U"{}/{}/{}";
        constexpr static auto DURATION    = U"{}:{:02}";
    }

    namespace Download
    {
        constexpr static auto INITIALIZING     = "[Initialization]";
        constexpr static auto CONNECTING       = "[Connecting to server]";
        constexpr static auto DOWNLOADING      = "[Downloading]";
        constexpr static auto RENAMING         = "[Changine a file name]";
        constexpr static auto COMPLETED        = "[Downloading of a file is completed.]";
        constexpr static auto CONNECT_FAILED   = "[Failed in connecting to the server]";
        constexpr static auto DISK_FULL        = "[Shortage of disk capacity]";
        constexpr static auto FILE_NOT_FOUND   = "[Cannot find the file specified]";
        constexpr static auto FAILED_NOTICE    = "An error occurred while downloading. Please try it again after a little while later.";
        constexpr static auto DISK_FULL_NOTICE = "Shortage of disk capacity. Please check the capacity.";
        constexpr static auto CONNECT_FAILED_NOTICE = "Failed in connecting to the server to download a file. Please try it again after a little while later.";

        constexpr static auto PROGRESS_COUNT   = U"{}/{}";
        constexpr static auto PROGRESS_ELAPSED = U"{:2}:{:02}:{:02}";
        constexpr static auto PROGRESS_PERCENT = U"{:04.1f}";
    }
}
