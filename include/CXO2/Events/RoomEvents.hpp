#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Models/Room.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <SFML/System/String.hpp>

#include <cstdint>
#include <string>

namespace Cx
{
    class StateRoom;

    struct ChartMetadata;
    struct CharacterInfoResponse;
    struct RoomListResponse;
    struct UserListResponse;
    struct CreateRoomRequest;
    struct CreateRoomResponse;
    struct JoinRoomResponse;
    struct RoomCreatedEventData;
    struct RoomMusicChangedEventData;
    struct RoomStateChangedEventData;
    struct RoomTitleChangedEventData;
    struct RoomUserCountChangedEventData;
    struct RoomRemovedEventData;

    enum class RoomEventKey
    {
        OnCreateRoom,
        OnJoinRoom,
        OnCharacterInfoLoad,
        OnRoomListLoad,
        OnUserListLoad,
        OnCreateRoomResponded,
        OnJoinRoomResponded,
        OnRoomCreated,
        OnRoomMusicChanged,
        OnRoomStateChanged,
        OnRoomTitleChanged,
        OnRoomUserCountChanged,
        OnRoomRemoved
    };

    struct RoomCreateEventArgs : Interceptable
    {
        sf::String& Title;
        GameMode& Mode;
        std::string& Password;
        unsigned int& MinLevelLimit;
        unsigned int& MaxLevelLimit;

        RoomCreateEventArgs(sf::String& title, GameMode& mode, std::string& password, unsigned int& minLevelLimit, unsigned int& maxLevelLimit) :
            Title(title),
            Mode(mode),
            Password(password),
            MinLevelLimit(minLevelLimit),
            MaxLevelLimit(maxLevelLimit)
        {
        }
    };

    struct RoomJoinEventArgs : Interceptable
    {
        const Room& Target;
        std::uint32_t& ID;
        std::string& Password;

        RoomJoinEventArgs(const Room& target, std::uint32_t& id, std::string& password) :
            Target(target),
            ID(id),
            Password(password)
        {
        }
    };

    struct RoomCharacterInfoEventArgs : Interceptable
    {
        const CharacterInfoResponse& Response;

        explicit RoomCharacterInfoEventArgs(const CharacterInfoResponse& response) : Response(response) {}
    };

    struct RoomListEventArgs : Interceptable
    {
        const RoomListResponse& Response;

        explicit RoomListEventArgs(const RoomListResponse& response) : Response(response) {}
    };

    struct RoomUserListEventArgs : Interceptable
    {
        const UserListResponse& Response;

        explicit RoomUserListEventArgs(const UserListResponse& response) : Response(response) {}
    };

    struct RoomCreateResponseEventArgs : Interceptable
    {
        const CreateRoomResponse& Response;
        const CreateRoomRequest& Request;
        const ChartMetadata& Music;

        RoomCreateResponseEventArgs(const CreateRoomResponse& response, const CreateRoomRequest& request, const ChartMetadata& music) :
            Response(response),
            Request(request),
            Music(music)
        {
        }
    };

    struct RoomJoinResponseEventArgs : Interceptable
    {
        const JoinRoomResponse& Response;

        explicit RoomJoinResponseEventArgs(const JoinRoomResponse& response) : Response(response) {}
    };

    struct RoomCreatedEventArgs : Interceptable
    {
        const RoomCreatedEventData& Data;

        explicit RoomCreatedEventArgs(const RoomCreatedEventData& data) : Data(data) {}
    };

    struct RoomMusicChangedEventArgs : Interceptable
    {
        const RoomMusicChangedEventData& Data;

        explicit RoomMusicChangedEventArgs(const RoomMusicChangedEventData& data) : Data(data) {}
    };

    struct RoomStateChangedEventArgs : Interceptable
    {
        const RoomStateChangedEventData& Data;

        explicit RoomStateChangedEventArgs(const RoomStateChangedEventData& data) : Data(data) {}
    };

    struct RoomTitleChangedEventArgs : Interceptable
    {
        const RoomTitleChangedEventData& Data;

        explicit RoomTitleChangedEventArgs(const RoomTitleChangedEventData& data) : Data(data) {}
    };

    struct RoomUserCountChangedEventArgs : Interceptable
    {
        const RoomUserCountChangedEventData& Data;

        explicit RoomUserCountChangedEventArgs(const RoomUserCountChangedEventData& data) : Data(data) {}
    };

    struct RoomRemovedEventArgs : Interceptable
    {
        const RoomRemovedEventData& Data;

        explicit RoomRemovedEventArgs(const RoomRemovedEventData& data) : Data(data) {}
    };

    using RoomCreateEvent           = Gx::Event<RoomEventKey, StateRoom, RoomCreateEventArgs>;
    using RoomJoinEvent             = Gx::Event<RoomEventKey, StateRoom, RoomJoinEventArgs>;
    using RoomCharacterInfoEvent    = Gx::Event<RoomEventKey, StateRoom, RoomCharacterInfoEventArgs>;
    using RoomListEvent             = Gx::Event<RoomEventKey, StateRoom, RoomListEventArgs>;
    using RoomUserListEvent         = Gx::Event<RoomEventKey, StateRoom, RoomUserListEventArgs>;
    using RoomCreateResponseEvent   = Gx::Event<RoomEventKey, StateRoom, RoomCreateResponseEventArgs>;
    using RoomJoinResponseEvent     = Gx::Event<RoomEventKey, StateRoom, RoomJoinResponseEventArgs>;
    using RoomCreatedEvent          = Gx::Event<RoomEventKey, StateRoom, RoomCreatedEventArgs>;
    using RoomMusicChangedEvent     = Gx::Event<RoomEventKey, StateRoom, RoomMusicChangedEventArgs>;
    using RoomStateChangedEvent     = Gx::Event<RoomEventKey, StateRoom, RoomStateChangedEventArgs>;
    using RoomTitleChangedEvent     = Gx::Event<RoomEventKey, StateRoom, RoomTitleChangedEventArgs>;
    using RoomUserCountChangedEvent = Gx::Event<RoomEventKey, StateRoom, RoomUserCountChangedEventArgs>;
    using RoomRemovedEvent          = Gx::Event<RoomEventKey, StateRoom, RoomRemovedEventArgs>;

    namespace RoomEvents
    {
        inline constexpr RoomCreateEvent           OnCreateRoom{RoomEventKey::OnCreateRoom};
        inline constexpr RoomJoinEvent             OnJoinRoom{RoomEventKey::OnJoinRoom};
        inline constexpr RoomCharacterInfoEvent    OnCharacterInfoLoad{RoomEventKey::OnCharacterInfoLoad};
        inline constexpr RoomListEvent             OnRoomListLoad{RoomEventKey::OnRoomListLoad};
        inline constexpr RoomUserListEvent         OnUserListLoad{RoomEventKey::OnUserListLoad};
        inline constexpr RoomCreateResponseEvent   OnCreateRoomResponded{RoomEventKey::OnCreateRoomResponded};
        inline constexpr RoomJoinResponseEvent     OnJoinRoomResponded{RoomEventKey::OnJoinRoomResponded};
        inline constexpr RoomCreatedEvent          OnRoomCreated{RoomEventKey::OnRoomCreated};
        inline constexpr RoomMusicChangedEvent     OnRoomMusicChanged{RoomEventKey::OnRoomMusicChanged};
        inline constexpr RoomStateChangedEvent     OnRoomStateChanged{RoomEventKey::OnRoomStateChanged};
        inline constexpr RoomTitleChangedEvent     OnRoomTitleChanged{RoomEventKey::OnRoomTitleChanged};
        inline constexpr RoomUserCountChangedEvent OnRoomUserCountChanged{RoomEventKey::OnRoomUserCountChanged};
        inline constexpr RoomRemovedEvent          OnRoomRemoved{RoomEventKey::OnRoomRemoved};
    }
}
