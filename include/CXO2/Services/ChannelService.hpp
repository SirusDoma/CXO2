#pragma once

#include <CXO2/Services/MessageService.hpp>
#include <CXO2/Services/CharacterService.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Messages/Responses/CreateRoomResponse.hpp>
#include <CXO2/Messages/Responses/JoinRoomResponse.hpp>
#include <CXO2/Messages/Responses/RoomListResponse.hpp>
#include <CXO2/Messages/Responses/UserListResponse.hpp>
#include <CXO2/Messages/Responses/ChannelLogoutResponse.hpp>

#include <CXO2/Messages/Events/RoomCreatedEventData.hpp>
#include <CXO2/Messages/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomRemovedEventData.hpp>
#include <CXO2/Messages/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomUserCountChangedEventData.hpp>

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

    class MessageService;
    class ChannelOnlineService : public ChannelService
    {
    public:
        explicit ChannelOnlineService(SessionContext& session, MessageService& messages);

        void GetChannelInfo(
            const MessageCallback<RoomListResponse>& roomCallback,
            const MessageCallback<UserListResponse>& userListCallback
        ) override;

        void GetUserList(
            const MessageCallback<UserListResponse>& callback
        ) override;

        void CreateRoom(
            const CreateRoomRequest& request,
            const MessageCallback<CreateRoomResponse>& callback
        ) override;

        void JoinRoom(
            const JoinRoomRequest& request,
            const MessageCallback<JoinRoomResponse>& callback
        ) override;

        void Logout(
            const MessageCallback<ChannelLogoutResponse>& callback
            ) override;

        void SetRoomCreatedEventCallback(
            const MessageCallback<RoomCreatedEventData>& callback
        ) override;

        void SetRoomMusicChangedEventCallback(
            const MessageCallback<RoomMusicChangedEventData>& callback
        ) override;

        void SetRoomStateChangedEventCallback(
            const MessageCallback<RoomStateChangedEventData>& callback
        ) override;

        void SetRoomTitleChangedEventCallback(
            const MessageCallback<RoomTitleChangedEventData>& callback
        ) override;

        void SetRoomUserCountChangedEventCallback(
            const MessageCallback<RoomUserCountChangedEventData>& callback
        ) override;

        void SetRoomRemovedEventCallback(
            const MessageCallback<RoomRemovedEventData>& callback
        ) override;

    private:
        SessionContext& m_session;
        MessageService& m_messages;

        MessageSubscriber<RoomCreatedEventData>          m_createSubscriber;
        MessageSubscriber<RoomMusicChangedEventData>     m_musicSubscriber;
        MessageSubscriber<RoomStateChangedEventData>     m_stateSubscriber;
        MessageSubscriber<RoomTitleChangedEventData>     m_titleSubscriber;
        MessageSubscriber<RoomUserCountChangedEventData> m_countSubscriber;
        MessageSubscriber<RoomRemovedEventData>          m_removeSubscriber;
    };

    class ChannelOfflineService : public ChannelService
    {
    public:
        void GetChannelInfo(
            const MessageCallback<RoomListResponse>& roomCallback,
            const MessageCallback<UserListResponse>& userListCallback
        ) override;
    };

}
