#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Services/MessageService.hpp>

#include <CXO2/Messages/Requests/AnnouncementRequest.hpp>
#include <CXO2/Messages/Requests/MainRoomMessageRequest.hpp>
#include <CXO2/Messages/Requests/WaitingMessageRequest.hpp>
#include <CXO2/Messages/Requests/WhisperMessageRequest.hpp>

#include <CXO2/Messages/Responses/WhisperMessageResponse.hpp>
#include <CXO2/Messages/Responses/MainRoomUserMessageResponse.hpp>
#include <CXO2/Messages/Responses/MainRoomAdminMessageResponse.hpp>
#include <CXO2/Messages/Responses/WaitingUserMessageResponse.hpp>
#include <CXO2/Messages/Responses/WaitingAdminMessageResponse.hpp>

#include <CXO2/Messages/Events/WhisperEventData.hpp>
#include <CXO2/Messages/Events/AnnouncementEventData.hpp>

#include <functional>

namespace Cx
{
    class MessagingService : public virtual Service
    {
    public:
        virtual void SendAnnouncement(
            const AnnouncementRequest& request,
            const MessageCallback<AnnouncementRequest>& callback = nullptr
        ) = 0;

        virtual void SendMainRoomMessage(
            const MainRoomMessageRequest& request,
            const MessageCallback<MainRoomMessageRequest>& callback = nullptr
        ) = 0;

        virtual void SendWaitingMessage(
            const WaitingMessageRequest& request,
            const MessageCallback<WaitingMessageRequest>& callback = nullptr
        ) = 0;

        virtual void SendWhisper(
            const WhisperMessageRequest& request,
            const MessageCallback<WhisperMessageResponse>& callback = nullptr
        ) = 0;

        virtual void SetWhisperEventCallback(
            const MessageCallback<WhisperEventData>& callback
        ) = 0;

        virtual void SetMainRoomUserMessageCallback(
            const MessageCallback<MainRoomUserMessageResponse>& callback
        ) = 0;

        virtual void SetMainRoomAdminMessageCallback(
            const MessageCallback<MainRoomAdminMessageResponse>& callback
        ) = 0;

        virtual void SetWaitingUserMessageCallback(
            const MessageCallback<WaitingUserMessageResponse>& callback
        ) = 0;

        virtual void SetWaitingAdminMessageCallback(
            const MessageCallback<WaitingAdminMessageResponse>& callback
        ) = 0;
    };

    class MessageService;
    class MessagingOnlineService : public MessagingService
    {
    public:
        explicit MessagingOnlineService(MessageService& messages);

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
        MessageService& m_messages;

        MessageSubscriber<WhisperEventData>             m_whisperSubscriber;
        MessageSubscriber<MainRoomUserMessageResponse>  m_mainRoomUserSubscriber;
        MessageSubscriber<MainRoomAdminMessageResponse> m_mainRoomAdminSubscriber;
        MessageSubscriber<WaitingUserMessageResponse>   m_waitingUserSubscriber;
        MessageSubscriber<WaitingAdminMessageResponse>  m_waitingAdminSubscriber;
        MessageSubscriber<AnnouncementEventData>        m_announcementSubscriber;
    };

    class MessagingOfflineService : public MessagingService
    {
    };
}
