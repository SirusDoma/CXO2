#pragma once

#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/Network/RoomInfo.hpp>

#include <vector>

namespace Cx
{
    class SessionContext;
    class ChannelOfflineService : public ChannelService
    {
    public:
        explicit ChannelOfflineService(SessionContext& session);

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

        std::vector<RoomInfo> m_rooms;

        MessageCallback<RoomCreatedEventData>          m_createCallback;
        MessageCallback<RoomMusicChangedEventData>     m_musicCallback;
        MessageCallback<RoomStateChangedEventData>     m_stateCallback;
        MessageCallback<RoomTitleChangedEventData>     m_titleCallback;
        MessageCallback<RoomUserCountChangedEventData> m_countCallback;
        MessageCallback<RoomRemovedEventData>          m_removeCallback;
    };
}
