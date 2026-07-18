#pragma once

#include <CXO2/Services/WaitingService.hpp>

namespace Cx
{
    class WaitingOfflineService : public WaitingService
    {
    public:
        void StartGame(
            const MessageCallback<StartGameRequest>& callback = nullptr
        ) const override;

        void UpdateReadyState(
            const MessageCallback<UpdateMemberReadyStateRequest>& callback = nullptr
        ) const override;

        void UpdateMusic(
            const UpdateRoomMusicRequest& request,
            const MessageCallback<UpdateRoomMusicRequest>& callback = nullptr
        ) const override;

        void UpdateRoomSlot(
            const UpdateRoomSlotRequest& request,
            const MessageCallback<UpdateRoomSlotRequest>& callback = nullptr
        ) const override;

        void UpdateTeam(
            const UpdateMemberTeamRequest& request,
            const MessageCallback<UpdateMemberTeamRequest>& callback = nullptr
        ) const override;

        void UpdateRoomTitle(
            const UpdateRoomTitleRequest& request,
            const MessageCallback<UpdateRoomTitleRequest>& callback = nullptr
        ) const override;

        void UpdateMap(
            const UpdateMapRequest& request,
            const MessageCallback<UpdateMapRequest>& callback = nullptr
        ) const override;

        void ExitRoom(
            const MessageCallback<ExitWaitingResponse>& callback = nullptr
        ) const override;

        void SetSlotChangedEventCallback(
            const MessageCallback<WaitingSlotChangedEventData>& callback
        ) override;

        void SetMemberJoinedEventCallback(
            const MessageCallback<WaitingMemberJoinedEventData>& callback
        ) override;

        void SetMemberLeftEventCallback(
            const MessageCallback<WaitingMemberLeftEventData>& callback
        ) override;

        void SetMemberTeamChangedEventCallback(
            const MessageCallback<WaitingMemberTeamChangedEventData>& callback
        ) override;

        void SetMemberReadyStateChangedEventCallback(
            const MessageCallback<WaitingMemberReadyStateChangedEventData>& callback
        ) override;

        void SetMusicChangedEventCallback(
            const MessageCallback<WaitingMusicChangedEventData>& callback
        ) override;

        void SetTitleChangedEventCallback(
            const MessageCallback<WaitingTitleChangedEventData>& callback
        ) override;

        void SetMapChangedEventCallback(
            const MessageCallback<WaitingMapChangedEventData>& callback
        ) override;

        void SetKickedEventCallback(
            const MessageCallback<WaitingKickEventData>& callback
        ) override;

        void SetStartGameEventCallback(
            const MessageCallback<StartGameEventData>& callback
        ) override;

    private:
        mutable bool m_ready{false};

        MessageCallback<WaitingSlotChangedEventData>             m_slotCallback;
        MessageCallback<WaitingMemberJoinedEventData>            m_joinCallback;
        MessageCallback<WaitingMemberLeftEventData>              m_leftCallback;
        MessageCallback<WaitingMemberTeamChangedEventData>       m_teamCallback;
        MessageCallback<WaitingMemberReadyStateChangedEventData> m_readyCallback;
        MessageCallback<WaitingMusicChangedEventData>            m_musicCallback;
        MessageCallback<WaitingTitleChangedEventData>            m_titleCallback;
        MessageCallback<WaitingMapChangedEventData>              m_mapCallback;
        MessageCallback<WaitingKickEventData>                    m_kickCallback;
        MessageCallback<StartGameEventData>                      m_startCallback;
    };
}
