#include <CXO2/Services/Online/ChannelOnlineService.hpp>
#include <CXO2/Services/NetworkService.hpp>

#include <CXO2/Network/Requests/ChannelInfoRequest.hpp>
#include <CXO2/Network/Requests/UserListRequest.hpp>
#include <CXO2/Network/Requests/CreateRoomRequest.hpp>
#include <CXO2/Network/Requests/JoinRoomRequest.hpp>
#include <CXO2/Network/Requests/ChannelLogoutRequest.hpp>

#include <CXO2/Network/Events/RoomCreatedEventData.hpp>
#include <CXO2/Network/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Network/Events/RoomRemovedEventData.hpp>
#include <CXO2/Network/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Network/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Network/Events/RoomUserCountChangedEventData.hpp>

namespace Cx
{
    ChannelOnlineService::ChannelOnlineService(SessionContext& session, NetworkService& network) :
        m_session(session),
        m_network(network)
    {
    }

    void ChannelOnlineService::GetChannelInfo(
        const MessageCallback<RoomListResponse>& roomCallback,
        const MessageCallback<UserListResponse>& userListCallback
    )
    {
        m_network.Dispatch<ChannelInfoRequest>(ChannelInfoRequest{}, [this, roomCallback, userListCallback] (const MessageEnvelope<ChannelInfoRequest>& result)
        {
            try
            {
                const auto& _ = result.Open();

                m_network.Acquire(roomCallback);
                m_network.Acquire(userListCallback);
            }
            catch (...)
            {
                if (roomCallback)
                    roomCallback(std::current_exception());
            }
        });
    }

    void ChannelOnlineService::GetUserList(const MessageCallback<UserListResponse>& callback)
    {
        m_network.Dispatch(UserListRequest{}, callback);
    }

    void ChannelOnlineService::CreateRoom(
        const CreateRoomRequest& request,
        const MessageCallback<CreateRoomResponse>& callback
    )
    {
        m_network.Dispatch(request, callback);
    }

    void ChannelOnlineService::JoinRoom(
        const JoinRoomRequest& request,
        const MessageCallback<JoinRoomResponse>& callback
    )
    {
        m_network.Dispatch(request, callback);
    }

    void ChannelOnlineService::Logout(const MessageCallback<ChannelLogoutResponse>& callback)
    {
        m_network.Dispatch(ChannelLogoutRequest{}, callback);
    }

    void ChannelOnlineService::SetRoomCreatedEventCallback(const MessageCallback<RoomCreatedEventData>& callback)
    {
        m_createSubscriber = m_network.On<RoomCreatedEventData>(callback);
    }

    void ChannelOnlineService::SetRoomMusicChangedEventCallback(const MessageCallback<RoomMusicChangedEventData>& callback)
    {
        m_musicSubscriber = m_network.On<RoomMusicChangedEventData>(callback);
    }

    void ChannelOnlineService::SetRoomStateChangedEventCallback(const MessageCallback<RoomStateChangedEventData>& callback)
    {
        m_stateSubscriber = m_network.On<RoomStateChangedEventData>(callback);
    }

    void ChannelOnlineService::SetRoomTitleChangedEventCallback(const MessageCallback<RoomTitleChangedEventData>& callback)
    {
        m_titleSubscriber = m_network.On<RoomTitleChangedEventData>(callback);
    }

    void ChannelOnlineService::SetRoomUserCountChangedEventCallback(const MessageCallback<RoomUserCountChangedEventData>& callback)
    {
        m_countSubscriber = m_network.On<RoomUserCountChangedEventData>(callback);
    }

    void ChannelOnlineService::SetRoomRemovedEventCallback(const MessageCallback<RoomRemovedEventData>& callback)
    {
        m_removeSubscriber = m_network.On<RoomRemovedEventData>(callback);
    }
}
