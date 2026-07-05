#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Services/MessageService.hpp>

#include <CXO2/Messages/Requests/StartGameRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMemberReadyStateRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomMusicRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomSlotRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMemberTeamRequest.hpp>
#include <CXO2/Messages/Requests/UpdateRoomTitleRequest.hpp>
#include <CXO2/Messages/Requests/UpdateMapRequest.hpp>

#include <CXO2/Messages/Responses/ExitWaitingResponse.hpp>

#include <CXO2/Messages/Events/WaitingSlotChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberJoinedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberLeftEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberTeamChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberReadyStateChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingTitleChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMapChangedEventData.hpp>
#include <CXO2/Messages/Events/WaitingKickEventData.hpp>
#include <CXO2/Messages/Events/StartGameEventData.hpp>

#include <functional>

namespace Cx
{
    class WaitingService : public virtual Service
    {
    public:
        virtual void StartGame(
            const MessageCallback<StartGameRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateReadyState(
            const MessageCallback<UpdateMemberReadyStateRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateMusic(
            const UpdateRoomMusicRequest& request,
            const MessageCallback<UpdateRoomMusicRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateRoomSlot(
            const UpdateRoomSlotRequest& request,
            const MessageCallback<UpdateRoomSlotRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateTeam(
            const UpdateMemberTeamRequest& request,
            const MessageCallback<UpdateMemberTeamRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateRoomTitle(
            const UpdateRoomTitleRequest& request,
            const MessageCallback<UpdateRoomTitleRequest>& callback = nullptr
        ) const = 0;

        virtual void UpdateMap(
            const UpdateMapRequest& request,
            const MessageCallback<UpdateMapRequest>& callback = nullptr
        ) const = 0;

        virtual void ExitRoom(
            const MessageCallback<ExitWaitingResponse>& callback = nullptr
        ) const = 0;

        virtual void SetSlotChangedEventCallback(
            const MessageCallback<WaitingSlotChangedEventData>& callback
        ) = 0;

        virtual void SetMemberJoinedEventCallback(
            const MessageCallback<WaitingMemberJoinedEventData>& callback
        ) = 0;

        virtual void SetMemberLeftEventCallback(
            const MessageCallback<WaitingMemberLeftEventData>& callback
        ) = 0;

        virtual void SetMemberTeamChangedEventCallback(
            const MessageCallback<WaitingMemberTeamChangedEventData>& callback
        ) = 0;

        virtual void SetMemberReadyStateChangedEventCallback(
            const MessageCallback<WaitingMemberReadyStateChangedEventData>& callback
        ) = 0;

        virtual void SetMusicChangedEventCallback(
            const MessageCallback<WaitingMusicChangedEventData>& callback
        ) = 0;

        virtual void SetTitleChangedEventCallback(
            const MessageCallback<WaitingTitleChangedEventData>& callback
        ) = 0;

        virtual void SetMapChangedEventCallback(
            const MessageCallback<WaitingMapChangedEventData>& callback
        ) = 0;

        virtual void SetKickedEventCallback(
            const MessageCallback<WaitingKickEventData>& callback
        ) = 0;

        virtual void SetStartGameEventCallback(
            const MessageCallback<StartGameEventData>& callback
        ) = 0;
    };

    class MessageService;
    class WaitingOnlineService : public WaitingService
    {
    public:
        explicit WaitingOnlineService(MessageService& messages);

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
        MessageService& m_messages;

        MessageSubscriber<WaitingSlotChangedEventData>             m_slotSubscriber;
        MessageSubscriber<WaitingMemberJoinedEventData>            m_joinSubscriber;
        MessageSubscriber<WaitingMemberLeftEventData>              m_leftSubscriber;
        MessageSubscriber<WaitingMemberTeamChangedEventData>       m_teamSubscriber;
        MessageSubscriber<WaitingMemberReadyStateChangedEventData> m_readySubscriber;
        MessageSubscriber<WaitingMusicChangedEventData>            m_musicSubscriber;
        MessageSubscriber<WaitingTitleChangedEventData>            m_titleSubscriber;
        MessageSubscriber<WaitingMapChangedEventData>              m_mapSubscriber;
        MessageSubscriber<WaitingKickEventData>                    m_kickSubscriber;
        MessageSubscriber<StartGameEventData>                      m_startSubscriber;
    };
}
