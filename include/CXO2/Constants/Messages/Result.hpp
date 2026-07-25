#pragma once

#include <array>

namespace Cx::Constants::Messages::Result
{
    constexpr static auto LEVEL      = "Level"; // Unmapped
    constexpr static auto UPGRADE_TO = "Upgrade to"; // Unmapped

    namespace Mission
    {
        constexpr static auto LABEL              = "Mission"; // Unmapped
        constexpr static auto SUCCEEDED          = "Mission succeeded!"; // Unmapped
        constexpr static auto FAILED             = "Mission failed"; // Unmapped
        constexpr static auto CONGRATULATIONS    = "Congratulations"; // Unmapped
        constexpr static auto LEVEL_ACHIEVED     = "Achieved the mission level"; // Unmapped
        constexpr static auto FAILED_NOTICE      = "Unless accomplished the mission presented, neither experience value nor level can be upgraeded"; // Unmapped
        constexpr static auto PENDING_NOTICE     = "Unless accomplishing the mission presented, neither experience value nor level can be upgraded"; // Unmapped
        constexpr static auto SUMMARY            = U"{:>18}LEVEL {} MISSION\n{}\n{}\n{}"; // Unmapped
        constexpr static auto SUMMARY_UPGRADED   = U"{:>18}LEVEL {} MISSION\n{}\n{}\n{} {}{}{}"; // Unmapped
        constexpr static auto PROGRESS           = U"{:>28}MISSION\n{}\n{} {} {} : {}\n{} "; // Unmapped

        constexpr static std::array REQUIREMENTS = { // Unmapped
            "To clear 50 combos or more of a tune at the minimum level 4",
            "To clear a tune with the minimum level 5 at half-speed",
            "To clear a tune with the minimum level 7 at a LIFE 100%",
            "To clear a tune with the minimum level 9 at COOL 70%",
            "To clear a tune with the minimum level 11 by gaining 15 JAMs or more",
            "To clear a tune with the minimum level 12 at COOL 80%",
            "To clear a tune with the minimum level 14 at six-fold-speed",
            "To clear a tune with the minimum level 16 by gaining 20 JAMs or more",
            "To clear a tune with the minimum level 16 at one-fold-speed at LIFE 100%",
            "To clear a tune with the minimum level 17 by 0 JAM",
            "To clear a tune with the minimum level 17 ALL COMBO",
            "To clear a tune with the minimum level 18 at COOL 80% or more"
        };
    }
}
