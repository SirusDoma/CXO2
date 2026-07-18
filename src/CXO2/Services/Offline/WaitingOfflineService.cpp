#include <CXO2/Services/Offline/WaitingOfflineService.hpp>

#include <CXO2/Network/Requests/StartGameRequest.hpp>
#include <CXO2/Network/Requests/UpdateMemberReadyStateRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Network/Requests/UpdateMapRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomSlotRequest.hpp>
#include <CXO2/Network/Requests/UpdateMemberTeamRequest.hpp>
#include <CXO2/Network/Requests/UpdateRoomTitleRequest.hpp>

namespace Cx
{
    void WaitingOfflineService::StartGame(const MessageCallback<StartGameRequest>& callback) const
    {
        if (m_startCallback)
            m_startCallback(MessageEnvelope<StartGameEventData>(StartGameEventData{ StartGameResult::Success }));

        if (callback)
            callback(MessageEnvelope<StartGameRequest>(StartGameRequest{}));
    }

    void WaitingOfflineService::UpdateReadyState(const MessageCallback<UpdateMemberReadyStateRequest>& callback) const
    {
        m_ready = !m_ready;
        if (m_readyCallback)
            m_readyCallback(MessageEnvelope<WaitingMemberReadyStateChangedEventData>(WaitingMemberReadyStateChangedEventData{ 0, m_ready }));

        if (callback)
            callback(MessageEnvelope<UpdateMemberReadyStateRequest>(UpdateMemberReadyStateRequest{}));
    }

    void WaitingOfflineService::UpdateMusic(
        const UpdateRoomMusicRequest& request,
        const MessageCallback<UpdateRoomMusicRequest>& callback
    ) const
    {
        if (m_musicCallback)
            m_musicCallback(MessageEnvelope<WaitingMusicChangedEventData>(WaitingMusicChangedEventData{ request.MusicID, request.Difficulty, request.SpeedID }));

        if (callback)
            callback(MessageEnvelope<UpdateRoomMusicRequest>(request));
    }

    void WaitingOfflineService::UpdateRoomSlot(
        const UpdateRoomSlotRequest& request,
        const MessageCallback<UpdateRoomSlotRequest>& callback
    ) const
    {
        if (m_slotCallback)
            m_slotCallback(MessageEnvelope<WaitingSlotChangedEventData>(WaitingSlotChangedEventData{ request.ID, RoomSlotEventType::Lock }));

        if (callback)
            callback(MessageEnvelope<UpdateRoomSlotRequest>(request));
    }

    void WaitingOfflineService::UpdateTeam(
        const UpdateMemberTeamRequest& request,
        const MessageCallback<UpdateMemberTeamRequest>& callback
    ) const
    {
        if (m_teamCallback)
            m_teamCallback(MessageEnvelope<WaitingMemberTeamChangedEventData>(WaitingMemberTeamChangedEventData{ 0, request.Team }));

        if (callback)
            callback(MessageEnvelope<UpdateMemberTeamRequest>(request));
    }

    void WaitingOfflineService::UpdateRoomTitle(
        const UpdateRoomTitleRequest& request,
        const MessageCallback<UpdateRoomTitleRequest>& callback
    ) const
    {
        if (m_titleCallback)
            m_titleCallback(MessageEnvelope<WaitingTitleChangedEventData>(WaitingTitleChangedEventData{ request.Title }));

        if (callback)
            callback(MessageEnvelope<UpdateRoomTitleRequest>(request));
    }

    void WaitingOfflineService::UpdateMap(
        const UpdateMapRequest& request,
        const MessageCallback<UpdateMapRequest>& callback
    ) const
    {
        if (m_mapCallback)
            m_mapCallback(MessageEnvelope<WaitingMapChangedEventData>(WaitingMapChangedEventData{ request.Map }));

        if (callback)
            callback(MessageEnvelope<UpdateMapRequest>(request));
    }

    void WaitingOfflineService::ExitRoom(const MessageCallback<ExitWaitingResponse>& callback) const
    {
        if (callback)
            callback(MessageEnvelope<ExitWaitingResponse>(ExitWaitingResponse{}));
    }

    void WaitingOfflineService::SetSlotChangedEventCallback(const MessageCallback<WaitingSlotChangedEventData>& callback)
    {
        m_slotCallback = callback;
    }

    void WaitingOfflineService::SetMemberJoinedEventCallback(const MessageCallback<WaitingMemberJoinedEventData>& callback)
    {
        m_joinCallback = callback;
    }

    void WaitingOfflineService::SetMemberLeftEventCallback(const MessageCallback<WaitingMemberLeftEventData>& callback)
    {
        m_leftCallback = callback;
    }

    void WaitingOfflineService::SetMemberTeamChangedEventCallback(const MessageCallback<WaitingMemberTeamChangedEventData>& callback)
    {
        m_teamCallback = callback;
    }

    void WaitingOfflineService::SetMemberReadyStateChangedEventCallback(const MessageCallback<WaitingMemberReadyStateChangedEventData>& callback)
    {
        m_readyCallback = callback;
    }

    void WaitingOfflineService::SetMusicChangedEventCallback(const MessageCallback<WaitingMusicChangedEventData>& callback)
    {
        m_musicCallback = callback;
    }

    void WaitingOfflineService::SetTitleChangedEventCallback(const MessageCallback<WaitingTitleChangedEventData>& callback)
    {
        m_titleCallback = callback;
    }

    void WaitingOfflineService::SetMapChangedEventCallback(const MessageCallback<WaitingMapChangedEventData>& callback)
    {
        m_mapCallback = callback;
    }

    void WaitingOfflineService::SetKickedEventCallback(const MessageCallback<WaitingKickEventData>& callback)
    {
        m_kickCallback = callback;
    }

    void WaitingOfflineService::SetStartGameEventCallback(const MessageCallback<StartGameEventData>& callback)
    {
        m_startCallback = callback;
    }
}
