#include <CXO2/Services/MessagingService.hpp>

#include <CXO2/States/State.hpp>

namespace Cx
{
    MessagingOnlineService::MessagingOnlineService(MessageService& messages) :
        m_messages(messages)
    {
        m_announcementSubscriber = m_messages.On<AnnouncementEventData>([] (const MessageEnvelope<AnnouncementEventData>& envelope)
        {
            try
            {
                State::Announce(envelope.Open().Content);
            }
            catch (const Gx::Exception&)
            {
            }
        });
    }

    void MessagingOnlineService::SendAnnouncement(const AnnouncementRequest& request, const MessageCallback<AnnouncementRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void MessagingOnlineService::SendMainRoomMessage(const MainRoomMessageRequest& request, const MessageCallback<MainRoomMessageRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void MessagingOnlineService::SendWaitingMessage(const WaitingMessageRequest& request, const MessageCallback<WaitingMessageRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void MessagingOnlineService::SendWhisper(
        const WhisperMessageRequest& request,
        const MessageCallback<WhisperMessageResponse>& callback
    )
    {
        m_messages.Dispatch<WhisperMessageRequest, WhisperMessageResponse>(request, callback);
    }

    void MessagingOnlineService::SetWhisperEventCallback(const MessageCallback<WhisperEventData>& callback)
    {
        m_whisperSubscriber = m_messages.On<WhisperEventData>(callback);
    }

    void MessagingOnlineService::SetMainRoomUserMessageCallback(const MessageCallback<MainRoomUserMessageResponse>& callback)
    {
        m_mainRoomUserSubscriber = m_messages.On<MainRoomUserMessageResponse>(callback);
    }

    void MessagingOnlineService::SetMainRoomAdminMessageCallback(const MessageCallback<MainRoomAdminMessageResponse>& callback)
    {
        m_mainRoomAdminSubscriber = m_messages.On<MainRoomAdminMessageResponse>(callback);
    }

    void MessagingOnlineService::SetWaitingUserMessageCallback(const MessageCallback<WaitingUserMessageResponse>& callback)
    {
        m_waitingUserSubscriber = m_messages.On<WaitingUserMessageResponse>(callback);
    }

    void MessagingOnlineService::SetWaitingAdminMessageCallback(const MessageCallback<WaitingAdminMessageResponse>& callback)
    {
        m_waitingAdminSubscriber = m_messages.On<WaitingAdminMessageResponse>(callback);
    }
}
