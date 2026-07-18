#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>

#include <CXO2/Network/Responses/CreateRoomResponse.hpp>
#include <CXO2/Network/Responses/JoinRoomResponse.hpp>
#include <CXO2/Network/Responses/RoomListResponse.hpp>
#include <CXO2/Network/Responses/UserListResponse.hpp>
#include <CXO2/Network/Responses/ChannelLogoutResponse.hpp>

#include <CXO2/Network/Events/RoomCreatedEventData.hpp>
#include <CXO2/Network/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Network/Events/RoomRemovedEventData.hpp>
#include <CXO2/Network/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Network/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Network/Events/RoomUserCountChangedEventData.hpp>

namespace Cx
{
    struct CreateRoomRequest;
    struct JoinRoomRequest;

    struct RoomInfo;

    class ChannelService : public virtual Service
    {
    public:
        virtual void GetChannelInfo(
            const MessageCallback<RoomListResponse>& roomCallback,
            const MessageCallback<UserListResponse>& userListCallback
        ) = 0;

        virtual void GetUserList(
            const MessageCallback<UserListResponse>& callback
        ) = 0;

        virtual void CreateRoom(
            const CreateRoomRequest& request,
            const MessageCallback<CreateRoomResponse>& callback
        ) = 0;

        virtual void JoinRoom(
            const JoinRoomRequest& request,
            const MessageCallback<JoinRoomResponse>& callback
        ) = 0;

        virtual void Logout(
            const MessageCallback<ChannelLogoutResponse>& callback
        ) = 0;

        virtual void SetRoomCreatedEventCallback(
            const MessageCallback<RoomCreatedEventData>& callback
        ) = 0;

        virtual void SetRoomMusicChangedEventCallback(
            const MessageCallback<RoomMusicChangedEventData>& callback
        ) = 0;

        virtual void SetRoomStateChangedEventCallback(
            const MessageCallback<RoomStateChangedEventData>& callback
        ) = 0;

        virtual void SetRoomTitleChangedEventCallback(
            const MessageCallback<RoomTitleChangedEventData>& callback
        ) = 0;

        virtual void SetRoomUserCountChangedEventCallback(
            const MessageCallback<RoomUserCountChangedEventData>& callback
        ) = 0;

        virtual void SetRoomRemovedEventCallback(
            const MessageCallback<RoomRemovedEventData>& callback
        ) = 0;
    };
}
