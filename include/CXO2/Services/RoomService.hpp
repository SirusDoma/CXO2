#pragma once

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Services/EventService.hpp>
#include <CXO2/Services/CharacterService.hpp>

namespace Cx
{
    struct CreateRoomRequest;
    struct JoinRoomRequest;

    struct RoomListResponse;
    struct UserListResponse;
    struct CreateRoomResponse;
    struct JoinRoomResponse;

    struct RoomMusicChangedEventData;
    struct RoomStateChangedEventData;
    struct RoomTitleChangedEventData;
    struct RoomUserCountChangedEventData;

    struct RoomInfo;

    class RoomService : public virtual Service
    {
    public:
        virtual void GetChannelInfo(
            std::function<void(const RoomListResponse&)> roomCallback,
            std::function<void(const UserListResponse&)> userListCallback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) = 0;

        virtual void GetUserList(
            std::function<void(const std::vector<CharacterInfo>&)> userListCallback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) = 0;

        virtual void CreateRoom(
            const CreateRoomRequest& request,
            const std::function<void(const CreateRoomResponse&)>& callback,
            const std::function<void(const NetworkException&)>& errorCallback
        ) = 0;

        virtual void JoinRoom(
            const JoinRoomRequest& request,
            const std::function<void(const JoinRoomResponse&)>& callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) = 0;

        virtual void ChannelLogout(
            std::function<void()> callback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) = 0;
    };

    class NetworkAdapter;
    class NetworkException;
    class RoomOnlineService : public RoomService, public EventService
    {
    public:
        explicit RoomOnlineService(NetworkAdapter& adapter, SessionContext& session);

        void GetChannelInfo(
            std::function<void(const RoomListResponse&)> roomCallback,
            std::function<void(const UserListResponse&)> userListCallback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) override;

        void GetUserList(
            std::function<void(const std::vector<CharacterInfo>&)> userListCallback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) override;

        void CreateRoom(
            const CreateRoomRequest& request,
            const std::function<void(const CreateRoomResponse&)>& callback,
            const std::function<void(const NetworkException&)>& errorCallback
        ) override;

        void JoinRoom(
            const JoinRoomRequest& request,
            const std::function<void(const JoinRoomResponse&)>& callback,
            const std::function<void(const NetworkException&)>& errorCallback = nullptr
        ) override;

        void ChannelLogout(
            std::function<void()> callback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) override;

    private:
        SessionContext& m_session;
    };

    class RoomOfflineService : public RoomService
    {
    public:
        void GetChannelInfo(
            std::function<void(const RoomListResponse&)> roomCallback,
            std::function<void(const UserListResponse&)> userListCallback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) override;
    };

}
