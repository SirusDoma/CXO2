#include <CXO2/Services/WaitingService.hpp>

#include <CXO2/Messages/Requests/StartGameRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMemberReadyStateRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMapRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomSlotRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMemberTeamRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomTitleRequest.hpp>
#include <CXO2/Messages/Requests/ExitWaitingRequest.hpp>

namespace Cx
{
    WaitingOnlineService::WaitingOnlineService(MessageService& messages) :
        m_messages(messages)
    {
    }

    void WaitingOnlineService::StartGame(const MessageCallback<StartGameRequest>& callback) const
    {
        m_messages.Dispatch(StartGameRequest{}, callback);
    }

    void WaitingOnlineService::UpdateReadyState(const MessageCallback<UpdateMemberReadyStateRequest>& callback) const
    {
        m_messages.Dispatch(UpdateMemberReadyStateRequest{}, callback);
    }

    void WaitingOnlineService::UpdateMusic(
        const UpdateRoomMusicRequest& request,
        const MessageCallback<UpdateRoomMusicRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateRoomSlot(
        const UpdateRoomSlotRequest& request,
        const MessageCallback<UpdateRoomSlotRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateTeam(
        const UpdateMemberTeamRequest& request,
        const MessageCallback<UpdateMemberTeamRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateRoomTitle(
        const UpdateRoomTitleRequest& request,
        const MessageCallback<UpdateRoomTitleRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void WaitingOnlineService::UpdateMap(
        const UpdateMapRequest& request,
        const MessageCallback<UpdateMapRequest>& callback
    ) const
    {
        m_messages.Dispatch(request, callback);
    }

    void WaitingOnlineService::ExitRoom(const MessageCallback<ExitWaitingResponse>& callback) const
    {
        m_messages.Dispatch<ExitWaitingRequest, ExitWaitingResponse>(ExitWaitingRequest{}, callback);
    }

    void WaitingOnlineService::SetSlotChangedEventCallback(const MessageCallback<WaitingSlotChangedEventData>& callback)
    {
        m_slotSubscriber = m_messages.On<WaitingSlotChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberJoinedEventCallback(const MessageCallback<WaitingMemberJoinedEventData>& callback)
    {
        m_joinSubscriber = m_messages.On<WaitingMemberJoinedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberLeftEventCallback(const MessageCallback<WaitingMemberLeftEventData>& callback)
    {
        m_leftSubscriber = m_messages.On<WaitingMemberLeftEventData>(callback);
    }

    void WaitingOnlineService::SetMemberTeamChangedEventCallback(const MessageCallback<WaitingMemberTeamChangedEventData>& callback)
    {
        m_teamSubscriber = m_messages.On<WaitingMemberTeamChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMemberReadyStateChangedEventCallback(const MessageCallback<WaitingMemberReadyStateChangedEventData>& callback)
    {
        m_readySubscriber = m_messages.On<WaitingMemberReadyStateChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMusicChangedEventCallback(const MessageCallback<WaitingMusicChangedEventData>& callback)
    {
        m_musicSubscriber = m_messages.On<WaitingMusicChangedEventData>(callback);
    }

    void WaitingOnlineService::SetTitleChangedEventCallback(const MessageCallback<WaitingTitleChangedEventData>& callback)
    {
        m_titleSubscriber = m_messages.On<WaitingTitleChangedEventData>(callback);
    }

    void WaitingOnlineService::SetMapChangedEventCallback(const MessageCallback<WaitingMapChangedEventData>& callback)
    {
        m_mapSubscriber = m_messages.On<WaitingMapChangedEventData>(callback);
    }

    void WaitingOnlineService::SetKickedEventCallback(const MessageCallback<WaitingKickEventData>& callback)
    {
        m_kickSubscriber = m_messages.On<WaitingKickEventData>(callback);
    }

    void WaitingOnlineService::SetStartGameEventCallback(const MessageCallback<StartGameEventData>& callback)
    {
        m_startSubscriber = m_messages.On<StartGameEventData>(callback);
    }
}
