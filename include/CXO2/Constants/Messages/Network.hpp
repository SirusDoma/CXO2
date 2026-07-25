#pragma once

namespace Cx::Constants::Messages::Network
{
    constexpr static auto CONNECT_FAILED       = "Failed in connecting to the server.";
    constexpr static auto CONNECT_FAILED_SHORT = "Failed in connecting to server"; // Unmapped
    constexpr static auto NO_RESPONSE          = "No response is received from the server."; // Unmapped
    constexpr static auto DISCONNECTED         = "Disconnected from server"; // Unmapped
    constexpr static auto UNSTABLE             = "Network is not in a good condition. Please try again a little while later.";
    constexpr static auto UNAVAILABLE          = "Network connection cannot be established. Please try again later."; // Unmapped
}
