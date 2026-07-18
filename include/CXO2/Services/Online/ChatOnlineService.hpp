#pragma once

#include <CXO2/Services/ChatService.hpp>
#include <CXO2/Services/NetworkService.hpp>

namespace Cx
{
    class ChatOnlineService : public ChatService
    {
    public:
        explicit ChatOnlineService(NetworkService& network);

        void SendAnnouncement(
            const AnnouncementRequest& request,
            const MessageCallback<AnnouncementRequest>& callback = nullptr
        ) override;

        void SendMainRoomMessage(
            const MainRoomMessageRequest& request,
            const MessageCallback<MainRoomMessageRequest>& callback = nullptr
        ) override;

        void SendWaitingMessage(
            const WaitingMessageRequest& request,
            const MessageCallback<WaitingMessageRequest>& callback = nullptr
        ) override;

        void SendWhisper(
            const WhisperMessageRequest& request,
            const MessageCallback<WhisperMessageResponse>& callback = nullptr
        ) override;

        void SetWhisperEventCallback(
            const MessageCallback<WhisperEventData>& callback
        ) override;

        void SetMainRoomUserMessageCallback(
            const MessageCallback<MainRoomUserMessageResponse>& callback
        ) override;

        void SetMainRoomAdminMessageCallback(
            const MessageCallback<MainRoomAdminMessageResponse>& callback
        ) override;

        void SetWaitingUserMessageCallback(
            const MessageCallback<WaitingUserMessageResponse>& callback
        ) override;

        void SetWaitingAdminMessageCallback(
            const MessageCallback<WaitingAdminMessageResponse>& callback
        ) override;

    private:
        NetworkService& m_network;

        MessageSubscriber<WhisperEventData>             m_whisperSubscriber;
        MessageSubscriber<MainRoomUserMessageResponse>  m_mainRoomUserSubscriber;
        MessageSubscriber<MainRoomAdminMessageResponse> m_mainRoomAdminSubscriber;
        MessageSubscriber<WaitingUserMessageResponse>   m_waitingUserSubscriber;
        MessageSubscriber<WaitingAdminMessageResponse>  m_waitingAdminSubscriber;
        MessageSubscriber<AnnouncementEventData>        m_announcementSubscriber;
    };
}
