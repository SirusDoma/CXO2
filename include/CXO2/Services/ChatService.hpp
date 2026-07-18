#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>

#include <CXO2/Network/Requests/AnnouncementRequest.hpp>
#include <CXO2/Network/Requests/MainRoomMessageRequest.hpp>
#include <CXO2/Network/Requests/WaitingMessageRequest.hpp>
#include <CXO2/Network/Requests/WhisperMessageRequest.hpp>

#include <CXO2/Network/Responses/WhisperMessageResponse.hpp>
#include <CXO2/Network/Responses/MainRoomUserMessageResponse.hpp>
#include <CXO2/Network/Responses/MainRoomAdminMessageResponse.hpp>
#include <CXO2/Network/Responses/WaitingUserMessageResponse.hpp>
#include <CXO2/Network/Responses/WaitingAdminMessageResponse.hpp>

#include <CXO2/Network/Events/WhisperEventData.hpp>
#include <CXO2/Network/Events/AnnouncementEventData.hpp>

#include <functional>

namespace Cx
{
    class ChatService : public virtual Service
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
}
