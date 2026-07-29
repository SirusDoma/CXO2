#pragma once

namespace Cx::Constants::Messages::Loading
{
    constexpr static auto TUNE_LOAD_TIMEOUT   = "Tune loading time is passed up. Please retry start"; // Unmapped
    constexpr static auto TUNE_READ_FAILED    = U"[ERROR]{} music file read.\n\nAfter delete {} file in music folder, download again the music"; // Unmapped
    constexpr static auto TUNE_NOT_FOUND      = U"{} cannot find file"; // Unmapped
    constexpr static auto MISSION_HEADER      = U"LEVEL {} MISSION"; // Unmapped
    constexpr static auto SPEED               = U"{:.1f} x"; // Unmapped
    constexpr static auto MISSION_NEEDS_OTHER_TUNE = "With the selected tune the mission cannot be accomplished"; // Unmapped
    constexpr static auto MISSION_NEEDS_MULTIPLAY  = "With the single mode the mission cannot be accomplished"; // Unmapped
}
