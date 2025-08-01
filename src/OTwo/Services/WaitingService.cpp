#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Messages/Events/StartGameEventData.hpp>
#include <OTwo/Messages/Events/WaitingMapChangedEventData.hpp>
#include <OTwo/Services/WaitingService.hpp>

#include <OTwo/Messages/Requests/UpdateMemberReadyStateRequest.hpp>
#include <OTwo/Messages/Requests/UpdateRoomMusicRequest.hpp>
#include <OTwo/Messages/Requests/UpdateMapRequest.hpp>
#include <OTwo/Messages/Requests/UpdateRoomSlotRequest.hpp>
#include <OTwo/Messages/Requests/UpdateMemberTeamRequest.hpp>
#include <OTwo/Messages/Requests/UpdateRoomTitleRequest.hpp>
#include <OTwo/Messages/Requests/ExitWaitingRequest.hpp>
#include <OTwo/Messages/Responses/ExitWaitingResponse.hpp>

#include <OTwo/Messages/Events/WaitingMemberJoinedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberLeftEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberTeamChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMusicChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingSlotChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingTitleChangedEventData.hpp>
#include <OTwo/Messages/Events/WaitingKickEventData.hpp>
#include <OTwo/Messages/Requests/StartGameRequest.hpp>

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

