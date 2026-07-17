#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/StringEnvelope.hpp>

#include <CXO2/Network/Auth.hpp>

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
