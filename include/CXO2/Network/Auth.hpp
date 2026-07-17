#pragma once

#include <CXO2/Network/StringEnvelope.hpp>
#include <Genode/Network/Packet.hpp>

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>

#include <cstdint>

namespace Cx
{
    enum class AuthResult : std::uint32_t
    {
        Success             = 0x00000000,
        NetworkError        = 0x00000003, // 03
        InsufficientBalance = 0x0000000A, // 10 or 0x0000000B/11 or 0x00000021/33
        MultiGamesSession   = 0x00000011, // 17
        IllegalUser         = 0x00000012, // 18
        DatabaseError       = 0xFFFFFF9B, // -101
        Banned              = 0xFFFFFFFB, // -5
        DuplicateSessions   = 0xFFFFFFFE, // -2,
        InvalidCredentials  = 0xFFFFFFFF, // -1,
    };

    enum class BillingCode
    {
        FM,
        DB, // Day-Block
        FD,
        LE,
        TH,
        TB, // Time-Block
        LD,
        LM
    };

    inline Gx::Packet& operator<<(Gx::Packet& packet, const BillingCode& billingCode)
    {
        packet << magic_enum::enum_name<BillingCode>(billingCode).data();
        return packet;
    }

    inline Gx::Packet& operator>>(Gx::Packet& packet, BillingCode& billingCode)
    {
        auto code = StringEnvelope(2, 2, false);
        packet >> code;

        auto data = code.GetContainer();
        if (const auto parsed = magic_enum::enum_cast<BillingCode>(data); parsed.has_value())
            billingCode = parsed.value();
        else
            throw Gx::InvalidOperationException(fmt::format("Billing Error( Code: {} )", data));

        return packet;
    }
}
