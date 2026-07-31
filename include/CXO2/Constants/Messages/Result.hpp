#pragma once

#include <array>

namespace Cx::Constants::Messages::Result
{
    constexpr static auto LEVEL      = "Level";
    constexpr static auto UPGRADE_TO = "Upgrade to";

    namespace Mission
    {
        constexpr static auto LABEL              = "Mission";
        constexpr static auto SUCCEEDED          = "Mission succeeded!";
        constexpr static auto FAILED             = "Mission failed";
        constexpr static auto CONGRATULATIONS    = "Congratulations";
        constexpr static auto LEVEL_ACHIEVED     = "Achieved the mission level";
        constexpr static auto FAILED_NOTICE      = "Unless accomplished the mission presented, neither experience value nor level can be upgraeded";
        constexpr static auto PENDING_NOTICE     = "Unless accomplishing the mission presented, neither experience value nor level can be upgraded";
        constexpr static auto SUMMARY_FAILED     = U"{:>18}LEVEL {} MISSION\n{}\n{}\n{}";
        constexpr static auto SUMMARY_UPGRADED   = U"{:>18}LEVEL {} MISSION\n{}\n{}\n{} {}{}{}";
        constexpr static auto SUMMARY_UNLOCK     = U"{:>28}MISSION\n{}\n{} {} {} : {}\n{} ";

        constexpr static std::array REQUIREMENTS = {
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
