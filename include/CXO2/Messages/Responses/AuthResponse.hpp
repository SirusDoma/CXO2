#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/StringEnvelope.hpp>

#include <CXO2/Messages/Auth.hpp>

namespace Cx
{
    struct AuthResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::Authorize;

        AuthResult       ResultCode{};
        Cx::BillingCode  BillingCode{};
        StringEnvelope<> Timestamp{12, 12, false};
        std::uint32_t    SubscriptionRemainingTimeMinutes{};
    };
}
