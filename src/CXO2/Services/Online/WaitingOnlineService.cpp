#include <CXO2/Services/Online/WaitingOnlineService.hpp>

#include <CXO2/Network/Requests/StartGameRequest.hpp>
#include <CXO2/Network/Requests/UpdateMemberReadyStateRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Network/Requests/UpdateMapRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomSlotRequest.hpp>
#include <CXO2/Network/Requests/UpdateMemberTeamRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomTitleRequest.hpp>
#include <CXO2/Network/Requests/ExitWaitingRequest.hpp>

namespace Cx
{
    WaitingOnlineService::WaitingOnlineService(NetworkService& network) :
        m_network(network)
    {
    }

    void WaitingOnlineService::StartGame(const MessageCallback<StartGameRequest>& callback) const
    {
        m_network.Dispatch(StartGameRequest{}, callback);
    }

    void WaitingOnlineService::UpdateReadyState(const MessageCallback<UpdateMemberReadyStateRequest>& callback) const
    {
        m_network.Dispatch(UpdateMemberReadyStateRequest{}, callback);
    }

    void WaitingOnlineService::UpdateMusic(
        const UpdateRoomMusicRequest& request,
        const MessageCallback<UpdateRoomMusicRequest>& callback
    ) const
    {
        m_network.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateRoomSlot(
        const UpdateRoomSlotRequest& request,
        const MessageCallback<UpdateRoomSlotRequest>& callback
    ) const
    {
        m_network.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateTeam(
        const UpdateMemberTeamRequest& request,
        const MessageCallback<UpdateMemberTeamRequest>& callback
    ) const
    {
        m_network.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateRoomTitle(
        const UpdateRoomTitleRequest& request,
        const MessageCallback<UpdateRoomTitleRequest>& callback
    ) const
    {
        m_network.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateMap(
        const UpdateMapRequest& request,
        const MessageCallback<UpdateMapRequest>& callback
    ) const
    {
        m_network.Dispatch(request, callback);
    }

    void WaitingOnlineService::ExitRoom(const MessageCallback<ExitWaitingResponse>& callback) const
    {
        m_network.Dispatch<ExitWaitingRequest, ExitWaitingResponse>(ExitWaitingRequest{}, callback);
    }

    void WaitingOnlineService::SetSlotChangedEventCallback(const MessageCallback<WaitingSlotChangedEventData>& callback)
    {
        m_slotSubscriber = m_network.On<WaitingSlotChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberJoinedEventCallback(const MessageCallback<WaitingMemberJoinedEventData>& callback)
    {
        m_joinSubscriber = m_network.On<WaitingMemberJoinedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberLeftEventCallback(const MessageCallback<WaitingMemberLeftEventData>& callback)
    {
        m_leftSubscriber = m_network.On<WaitingMemberLeftEventData>(callback);
    }

    void WaitingOnlineService::SetMemberTeamChangedEventCallback(const MessageCallback<WaitingMemberTeamChangedEventData>& callback)
    {
        m_teamSubscriber = m_network.On<WaitingMemberTeamChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberReadyStateChangedEventCallback(const MessageCallback<WaitingMemberReadyStateChangedEventData>& callback)
    {
        m_readySubscriber = m_network.On<WaitingMemberReadyStateChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMusicChangedEventCallback(const MessageCallback<WaitingMusicChangedEventData>& callback)
    {
        m_musicSubscriber = m_network.On<WaitingMusicChangedEventData>(callback);
    }

    void WaitingOnlineService::SetTitleChangedEventCallback(const MessageCallback<WaitingTitleChangedEventData>& callback)
    {
        m_titleSubscriber = m_network.On<WaitingTitleChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMapChangedEventCallback(const MessageCallback<WaitingMapChangedEventData>& callback)
    {
        m_mapSubscriber = m_network.On<WaitingMapChangedEventData>(callback);
    }

    void WaitingOnlineService::SetKickedEventCallback(const MessageCallback<WaitingKickEventData>& callback)
    {
        m_kickSubscriber = m_network.On<WaitingKickEventData>(callback);
    }

    void WaitingOnlineService::SetStartGameEventCallback(const MessageCallback<StartGameEventData>& callback)
    {
        m_startSubscriber = m_network.On<StartGameEventData>(callback);
    }
}
