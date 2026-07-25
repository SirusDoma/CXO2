#pragma once

namespace Cx::Constants::Messages::Planet
{
    constexpr static auto CHANNEL_FULL                   = "Exceeding the channel (server) capacity, please enter another channel (server)";
    constexpr static auto EXIT_CONFIRM                   = "Do you end the game?";
    constexpr static auto EXIT_CONFIRM_WHILE_DOWNLOADING = "A tune is now being downloaded. Do you want to cancel it and end the game?"; // Unmapped

    namespace Authentication
    {
        constexpr static auto INVALID_CREDENTIALS = "Either login name or password is incorrect.";
        constexpr static auto ILLEGAL_USER        = "You are illegal user.";
        constexpr static auto ALREADY_CONNECTED   = "You have already connected another game.";
        constexpr static auto CONNECTING          = "User is now being connected to the Game.";
        constexpr static auto INSUFFICIENT_POINTS = "You have insufficient points to play. Please top up";
        constexpr static auto BANNED              = "You have been banned! \nPlease enquire customer service for detail.";
        constexpr static auto DATABASE_ERROR      = "(Please inquire of Administrator.) DB error";
        constexpr static auto NETWORK_ERROR       = "Network Error has occurred.";
        constexpr static auto BILLING_ERROR       = U"Billing Error( Code : {} )"; // Unused
        constexpr static auto BILL_METHOD_ERROR   = U"Bill Method Error( Code : {} )"; // Unused
    }

    namespace Session
    {
        constexpr static auto TIME_BLOCK_EXHAUSTED = "You have run out of e-Games points for Time-Block charging. Please top up your account to continue playing."; // Unused
        constexpr static auto DAY_BLOCK_EXPIRED    = "Your Day-Block subscription has expired. You will now be logged out from this game."; // Unused
        constexpr static auto DAY_BLOCK_EXPIRING   = U"Your Day-Block subscription will expire in {} minutes."; // Unused
        constexpr static auto RELOGIN_REQUIRED     = "Please go to Home Page and login again."; // Unused
        constexpr static auto DUPLICATE_LOGIN      = "You are disconnected due to double login from other PC."; // Unused
    }
}
