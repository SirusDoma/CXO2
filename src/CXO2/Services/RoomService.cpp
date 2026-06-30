#include <CXO2/Services/RoomService.hpp>

#include <CXO2/Messages/Requests/ChannelInfoRequest.hpp>
#include <CXO2/Messages/Requests/UserListRequest.hpp>
#include <CXO2/Messages/Requests/CreateRoomRequest.hpp>
#include <CXO2/Messages/Requests/JoinRoomRequest.hpp>
#include <CXO2/Messages/Requests/ChannelLogoutRequest.hpp>

#include <CXO2/Messages/Responses/RoomListResponse.hpp>
#include <CXO2/Messages/Responses/UserListResponse.hpp>
#include <CXO2/Messages/Responses/CreateRoomResponse.hpp>
#include <CXO2/Messages/Responses/JoinRoomResponse.hpp>
#include <CXO2/Messages/Responses/ChannelLogoutResponse.hpp>

#include <CXO2/Messages/Events/RoomCreatedEventData.hpp>
#include <CXO2/Messages/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomRemovedEventData.hpp>
#include <CXO2/Messages/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomUserCountChangedEventData.hpp>

#include <fmt/format.h>

namespace Cx
{
    RoomOnlineService::RoomOnlineService(NetworkAdapter& adapter, SessionContext& session) :
        EventService(adapter),
        m_session(session)
    {
    }

    void RoomOnlineService::GetChannelInfo(
        std::function<void(const RoomListResponse&)> roomCallback,
        std::function<void(const UserListResponse&)> userListCallback,
        const std::function<void(const NetworkException&)> errorCallback
    )
    {
        GetNetworkAdapter().SendAsync(ChannelInfoRequest{}, [this, roomCallback, userListCallback, errorCallback]
        {
            GetNetworkAdapter().ReceiveAsync<RoomListResponse>([this, roomCallback] (const auto& response)
            {
                if (roomCallback)
                    roomCallback(response);

            }, errorCallback);

            GetNetworkAdapter().ReceiveAsync<UserListResponse>([this, userListCallback] (const auto& response)
            {
                if (userListCallback)
                    userListCallback(response);

            }, errorCallback);
        });
    }

    void RoomOnlineService::GetUserList(std::function<void(const std::vector<CharacterInfo>&)> userListCallback,
        const std::function<void(const NetworkException&)> errorCallback)
    {
        GetNetworkAdapter().Exchange<UserListRequest, UserListResponse>(
            UserListRequest{},
            [this, userListCallback] (const auto& response)
            {
                auto list = std::vector<CharacterInfo>();
                for (const auto& user : response.Users.GetContainer())
                    list.push_back(CharacterInfo{ user.Name, Gender::Any, Role::Normal, user.Level });

                std::sort(list.begin(), list.end(), [] (const auto& a, const auto& b) { return a.Level > b.Level; });
                if (userListCallback)
                    userListCallback(list);

            },
            errorCallback
        );
    }

    void RoomOnlineService::CreateRoom(
        const CreateRoomRequest& request,
        const std::function<void(const CreateRoomResponse&)>& callback,
        const std::function<void(const NetworkException&)>& errorCallback
    )
    {
        GetNetworkAdapter().Exchange<CreateRoomRequest, CreateRoomResponse>(
            request,
            [=] (const auto& response)
            {
                if (callback)
                    callback(response);
            },
            errorCallback
        );
    }

    void RoomOnlineService::JoinRoom(
        const JoinRoomRequest& request,
        const std::function<void(const JoinRoomResponse&)>& callback,
        const std::function<void(const NetworkException&)>& errorCallback
    )
    {
        GetNetworkAdapter().Exchange<JoinRoomRequest, JoinRoomResponse>(
            request,
            [=] (const auto& response)
            {
                if (callback)
                    callback(response);
            },
            errorCallback
        );
    }

    void RoomOnlineService::ChannelLogout(std::function<void()> callback,
                                    const std::function<void(const NetworkException&)> errorCallback)
    {
        GetNetworkAdapter().Exchange<ChannelLogoutRequest, ChannelLogoutResponse>(
            ChannelLogoutRequest{},
            [this, callback] (const auto&)
            {
                GetNetworkAdapter().StopHeartbeat();
                if (callback)
                    callback();
            },
            errorCallback
        );
    }

    void RoomOfflineService::GetChannelInfo(
        std::function<void(const RoomListResponse&)> roomCallback,
        std::function<void(const UserListResponse&)> userListCallback,
        std::function<void(const NetworkException&)> errorCallback
    )
    {
        if (roomCallback)
        {
            roomCallback(RoomListResponse
            {
                std::vector
                {
                    RoomInfo
                    {
                        /* .ID            = */ 0,
                        /* .State         = */ RoomState::Playing,
                        /* .Title         = */std::string("Let's play together~"),
                        /* .Locked        = */ false,
                        /* .MusicID       = */ 100,
                        /* .Difficulty    = */ Difficulty::HX,
                        /* .Mode          = */ GameMode::Versus,
                        /* .SpeedID       = */ 1,
                        /* .Capacity      = */ 8,
                        /* .UserCount     = */ 2,
                        /* .MinLevelLimit = */ 0,
                        /* .MaxLevelLimit = */ 0
                    },
                    RoomInfo
                    {
                        /* .ID            = */ 5,
                        /* .State         = */ RoomState::Waiting,
                        /* .Title         = */ std::string("Another's room"),
                        /* .Locked        = */ false,
                        /* .MusicID       = */ 100,
                        /* .Difficulty    = */ Difficulty::EX,
                        /* .Mode          = */ GameMode::Single,
                        /* .SpeedID       = */ 1,
                        /* .Capacity      = */ 8,
                        /* .UserCount     = */ 2,
                        /* .MinLevelLimit = */ 0,
                        /* .MaxLevelLimit = */ 0
                    },

                    RoomInfo
                    {
                        /* .ID            = */ 3,
                        /* .State         = */ RoomState::Waiting,
                        /* .Title         = */ std::string("Someone's room"),
                        /* .Locked        = */ false,
                        /* .MusicID       = */ 100,
                        /* .Difficulty    = */ Difficulty::HX,
                        /* .Mode          = */ GameMode::Versus,
                        /* .SpeedID       = */ 1,
                        /* .Capacity      = */ 8,
                        /* .UserCount     = */ 1,
                        /* .MinLevelLimit = */ 20,
                        /* .MaxLevelLimit = */ 80
                    }
                }
            });
        }

        if (userListCallback)
        {
            auto list = std::vector<UserListResponse::UserInfo>();
            for (unsigned int i = 0; i < 34; i++)
                list.push_back({fmt::format("Dummy {}", i), static_cast<signed short>(i) });

            userListCallback(UserListResponse{ list });
        }
    }
}
