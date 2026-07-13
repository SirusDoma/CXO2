#include <CXO2/Services/ChatService.hpp>

#include <CXO2/States/State.hpp>

namespace Cx
{
    ChatOnlineService::ChatOnlineService(MessageService& messages) :
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

    void ChatOnlineService::SendAnnouncement(const AnnouncementRequest& request, const MessageCallback<AnnouncementRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void ChatOnlineService::SendMainRoomMessage(const MainRoomMessageRequest& request, const MessageCallback<MainRoomMessageRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void ChatOnlineService::SendWaitingMessage(const WaitingMessageRequest& request, const MessageCallback<WaitingMessageRequest>& callback)
    {
        m_messages.Dispatch(request, callback);
    }

    void ChatOnlineService::SendWhisper(
        const WhisperMessageRequest& request,
        const MessageCallback<WhisperMessageResponse>& callback
    )
    {
        m_messages.Dispatch<WhisperMessageRequest, WhisperMessageResponse>(request, callback);
    }

    void ChatOnlineService::SetWhisperEventCallback(const MessageCallback<WhisperEventData>& callback)
    {
        m_whisperSubscriber = m_messages.On<WhisperEventData>(callback);
    }

    void ChatOnlineService::SetMainRoomUserMessageCallback(const MessageCallback<MainRoomUserMessageResponse>& callback)
    {
        m_mainRoomUserSubscriber = m_messages.On<MainRoomUserMessageResponse>(callback);
    }

    void ChatOnlineService::SetMainRoomAdminMessageCallback(const MessageCallback<MainRoomAdminMessageResponse>& callback)
    {
        m_mainRoomAdminSubscriber = m_messages.On<MainRoomAdminMessageResponse>(callback);
    }

    void ChatOnlineService::SetWaitingUserMessageCallback(const MessageCallback<WaitingUserMessageResponse>& callback)
    {
        m_waitingUserSubscriber = m_messages.On<WaitingUserMessageResponse>(callback);
    }

    void ChatOnlineService::SetWaitingAdminMessageCallback(const MessageCallback<WaitingAdminMessageResponse>& callback)
    {
        m_waitingAdminSubscriber = m_messages.On<WaitingAdminMessageResponse>(callback);
    }
}
