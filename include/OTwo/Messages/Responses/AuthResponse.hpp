#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/StringEnvelope.hpp>

#include <OTwo/Messages/Auth.hpp>

struct AuthResponse
{
    static constexpr ::Command Command = ResponseCommand::Authorize;

    AuthResult       ResultCode{};
    ::BillingCode    BillingCode{};
    StringEnvelope<> Timestamp{12, 12, false};
    std::uint32_t    SubscriptionRemainingTimeMinutes{};
};
