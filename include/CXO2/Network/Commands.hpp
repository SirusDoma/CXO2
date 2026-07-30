#pragma once

#include <CXO2/Network/EnumEncoder.hpp>
#include <CXO2/Network/StringEncoder.hpp>

#include <Genode/Network/Packet.hpp>
#include <Genode/System/Exception.hpp>

#include <cstdint>
#include <variant>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    enum class RequestCommand : std::uint16_t
    {
        Authorize           = 0x03E8, // 1000
        GetChannelList      = 0x03EA, // 1002
        ChannelLogin        = 0x03EC, // 1004
        GetCharacterInfo    = 0x07D0, // 2000
        GetChannelInfo      = 0x07D2, // 2002
        CreateRoom          = 0x07D4, // 2004
        GetUserList         = 0x07DA, // 2010
        SendMainRoomMessage = 0x07DC, // 2012
        Announce            = 0x07DF, // 2015
        SendWhisper         = 0x07E1, // 2017
        ChannelLogout       = 0x07E5, // 2021
        SendMusicList       = 0x07E8, // 2024
        SetRoomTitle        = 0x0BB8, // 3000
        JoinWaiting         = 0x0BBA, // 3002
        ExitWaiting         = 0x0BBD, // 3005
        UpdateSlot          = 0x0BC0, // 3008
        SendWaitingMessage  = 0x0BC3, // 3011
        SetWaitingMusic     = 0x0FA0, // 4000
        SetWaitingMap       = 0x0FA2, // 4002
        SetUserTeam         = 0x0FA4, // 4004
        SetInstrument       = 0x0FA6, // 4006
        Ready               = 0x0FA8, // 4008
        StartGame           = 0x0FAA, // 4010
        ConfirmMusicLoaded  = 0x0FAC, // 4012
        UpdateGameStats     = 0x0FAE, // 4014
        ExitPlaying         = 0x0FB5, // 4021
        SubmitScore         = 0x0FB0, // 4016
        PurchaseItem        = 0x1388, // 5000
        SellItem            = 0x138A, // 5002
        EquipItem           = 0x138C, // 5004
        EnterShop           = 0x138E, // 5006
        ExitShop            = 0x138F, // 5007
        PurchaseMusic       = 0x1392, // 5010
        SyncMusicDownload   = 0x1394, // 5012
        Terminate           = 0xFFF0  // -16
    };

    enum class ResponseCommand : std::uint16_t
    {
        SubscriptionAlert    = 0x0032, // 0050
        ForceReauthorize     = 0x0033, // 0051
        MultiSessionError    = 0x0034, // 0052
        Authorize            = 0x03E9, // 1001
        GetChannelList       = 0x03EB, // 1003
        ChannelLogin         = 0x03ED, // 1005
        GetCharacterInfo     = 0x07D1, // 2001
        GetRoomList          = 0x07D3, // 2003
        CreateRoom           = 0x07D6, // 2006
        GetUserList          = 0x07DB, // 2011
        MainRoomUserMessage  = 0x07DD, // 2013
        MainRoomAdminMessage = 0x07DE, // 2014
        Announcement         = 0x07E0, // 2016
        SendWhisper          = 0x07E2, // 2018
        ChannelLogout        = 0x07E6, // 2022
        JoinRoom             = 0x0BBB, // 3003
        ExitWaiting          = 0x0BBE, // 3006
        WaitingUserMessage   = 0x0BC4, // 3012
        WaitingAdminMessage  = 0x0BC5, // 3013
        EquipItem            = 0x138D, // 5004
        PurchaseItem         = 0x1389, // 5001
        SellItem             = 0x138B, // 5003
        PurchaseMusic        = 0x1393, // 5011
    };

    enum class EventCommand : std::uint16_t
    {
        RoomCreated            = 0x07D5, // 2005
        RoomRemoved            = 0x07D7, // 2007
        RoomTitleChanged       = 0x07D8, // 2008
        RoomUserCountChanged   = 0x07D9, // 2009
        ReceiveWhisper         = 0x07E3, // 2019
        RoomStateChanged       = 0x07E4, // 2020
        RoomMusicChanged       = 0x07E7, // 2023
        WaitingTitleChanged    = 0x0BB9, // 3001
        UserJoinWaiting        = 0x0BBC, // 3004
        UserLeaveWaiting       = 0x0BBF, // 3007
        RoomSlotChanged        = 0x0BC1, // 3009
        Kick                   = 0x0BC2, // 3010
        RoomForceRemoved       = 0x0BC6, // 3013
        WaitingMusicChanged    = 0x0FA1, // 4001
        WaitingMapChanged      = 0x0FA3, // 4003
        UserTeamChanged        = 0x0FA5, // 4005
        UserInstrumentChanged  = 0x0FA7, // 4007
        UserReadyStateChanged  = 0x0FA9, // 4009
        MusicLoaded            = 0x0FAD, // 4013
        StartGame              = 0x0FAB, // 4011
        GameStatsUpdate        = 0x0FAF, // 4015
        ScoreSubmission        = 0x0FB1, // 4017
        GameCompleted          = 0x0FB2, // 4018
        UserLeaveGame          = 0x0FB6, // 4022
    };

    enum class GenericCommand : std::uint16_t
    {
        LegacyPing = 0x1770, // 6000
        Ping       = 0x1771, // 6001
    };
}

template <>
struct magic_enum::customize::enum_range<Cx::RequestCommand>
{
    static constexpr int min = 1000;
    static constexpr int max = 8000;
};

template <>
struct magic_enum::customize::enum_range<Cx::ResponseCommand>
{
    static constexpr int min = 0;
    static constexpr int max = 8000;
};

template <>
struct magic_enum::customize::enum_range<Cx::EventCommand>
{
    static constexpr int min = 1000;
    static constexpr int max = 8000;
};

template <>
struct magic_enum::customize::enum_range<Cx::GenericCommand>
{
    static constexpr int min = 6000;
    static constexpr int max = 7000;
};

namespace Cx
{
    class Command : public std::variant<RequestCommand, ResponseCommand, EventCommand, GenericCommand>
    {
    public:
        using std::variant<RequestCommand, ResponseCommand, EventCommand, GenericCommand>::variant;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        Command(const std::uint16_t value)
            : std::variant<RequestCommand, ResponseCommand, EventCommand, GenericCommand>(Parse(value))
        {
        }

        std::uint16_t code() const
        {
            return std::visit([](auto&& cmd)
            {
                return static_cast<std::uint16_t>(cmd);
            }, std::variant<RequestCommand, ResponseCommand, EventCommand, GenericCommand>(*this));
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        operator std::uint16_t() const
        {
            return code();
        }

        static std::variant<RequestCommand, ResponseCommand, EventCommand, GenericCommand> Parse(const std::uint16_t value)
        {
            // Special handling for minus value
            if (static_cast<RequestCommand>(value) == RequestCommand::Terminate)
                return RequestCommand::Terminate;

            if (auto cmd = magic_enum::enum_cast<RequestCommand>(value); cmd.has_value())
                return cmd.value();

            if (auto cmd = magic_enum::enum_cast<ResponseCommand>(value); cmd.has_value())
                return cmd.value();

            if (const auto cmd = magic_enum::enum_cast<EventCommand>(value); cmd.has_value())
                return cmd.value();

            if (const auto cmd = magic_enum::enum_cast<GenericCommand>(value); cmd.has_value())
                return cmd.value();

            throw Gx::ArgumentException("Specified command value is not recognized" + std::to_string(value));
        }
    };

    template<typename T, typename = void>
    struct IsMessage : std::false_type {};

    template<typename T>
    struct IsMessage<T, std::void_t<decltype(T::Command)>> 
        : std::bool_constant<std::is_same_v<decltype(T::Command), const Command>> {};

    inline Gx::Packet& operator>>(Gx::Packet& packet, Command& cmd)
    {
        std::uint16_t code{};
        packet >> code;

        auto result = Command::Parse(code);
        cmd.swap(result);

        return packet;
    }

    inline Gx::Packet& operator<<(Gx::Packet& packet, const Command& cmd)
    {
        packet << cmd.code();
        return packet;
    }
}
