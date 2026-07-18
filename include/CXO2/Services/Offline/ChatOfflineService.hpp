#pragma once

#include <CXO2/Services/ChatService.hpp>

namespace Cx
{
    class SessionContext;
    class ChatOfflineService : public ChatService
    {
    public:
        explicit ChatOfflineService(SessionContext& session);

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
        SessionContext& m_session;

        MessageCallback<WhisperEventData>             m_whisperCallback;
        MessageCallback<MainRoomUserMessageResponse>  m_mainRoomUserCallback;
        MessageCallback<MainRoomAdminMessageResponse> m_mainRoomAdminCallback;
        MessageCallback<WaitingUserMessageResponse>   m_waitingUserCallback;
        MessageCallback<WaitingAdminMessageResponse>  m_waitingAdminCallback;
    };
}
