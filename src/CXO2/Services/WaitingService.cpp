#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Messages/Events/StartGameEventData.hpp>
#include <CXO2/Messages/Events/WaitingMapChangedEventData.hpp>
#include <CXO2/Services/WaitingService.hpp>

#include <CXO2/Messages/Requests/UpdateMemberReadyStateRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMapRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomSlotRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMemberTeamRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomTitleRequest.hpp>
#include <CXO2/Messages/Requests/ExitWaitingRequest.hpp>
#include <CXO2/Messages/Responses/ExitWaitingResponse.hpp>

#include <CXO2/Messages/Events/WaitingMemberJoinedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberLeftEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberTeamChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingSlotChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingTitleChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingKickEventData.hpp>
#include <CXO2/Messages/Requests/StartGameRequest.hpp>

namespace Cx
{
    WaitingOnlineService::WaitingOnlineService(NetworkAdapter& adapter, SessionContext& session) :
        EventService(adapter),
        m_session(session)
    {
    }

    void WaitingOnlineService::StartGame(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<StartGameRequest>(
            StartGameRequest{},
            [callback]
            {
                if (callback)
                    callback();
            },
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateReadyState(
        const std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<UpdateMemberReadyStateRequest>(
            UpdateMemberReadyStateRequest{},
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateMusic(
        const UpdateRoomMusicRequest& request,
        const std::function<void()> callback, const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<UpdateRoomMusicRequest>(
            request,
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateRoomSlot(
        const std::uint8_t id,
        const std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback) const
    {
        GetNetworkAdapter().SendAsync<UpdateRoomSlotRequest>(
            UpdateRoomSlotRequest{id},
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateTeam(
        const RoomTeam team,
        const std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<UpdateMemberTeamRequest>(
            UpdateMemberTeamRequest{team},
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateRoomTitle(
        const std::string& title,
        const std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<UpdateRoomTitleRequest>(
            UpdateRoomTitleRequest{title},
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::UpdateMap(
        const UpdateMapRequest& request,
        const std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().SendAsync<UpdateMapRequest>(
            request,
            callback,
            errorCallback
        );
    }

    void WaitingOnlineService::ExitRoom(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback
    ) const
    {
        GetNetworkAdapter().Exchange<ExitWaitingRequest, ExitWaitingResponse>(
            ExitWaitingRequest{},
            [callback] (const auto&)
            {
                if (callback)
                    callback();
            },
            errorCallback
        );
    }

}
