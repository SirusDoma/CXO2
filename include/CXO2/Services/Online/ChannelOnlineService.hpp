#pragma once

#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/Services/NetworkService.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

namespace Cx
{
    class ChannelOnlineService : public ChannelService
    {
    public:
        explicit ChannelOnlineService(SessionContext& session, NetworkService& network);

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
        NetworkService& m_network;

        MessageSubscriber<RoomCreatedEventData>          m_createSubscriber;
        MessageSubscriber<RoomMusicChangedEventData>     m_musicSubscriber;
        MessageSubscriber<RoomStateChangedEventData>     m_stateSubscriber;
        MessageSubscriber<RoomTitleChangedEventData>     m_titleSubscriber;
        MessageSubscriber<RoomUserCountChangedEventData> m_countSubscriber;
        MessageSubscriber<RoomRemovedEventData>          m_removeSubscriber;
    };
}
