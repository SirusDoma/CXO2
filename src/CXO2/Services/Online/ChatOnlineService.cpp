#include <CXO2/Services/Online/ChatOnlineService.hpp>

#include <CXO2/States/State.hpp>

namespace Cx
{
    ChatOnlineService::ChatOnlineService(NetworkService& network) :
        m_network(network)
    {
        m_announcementSubscriber = m_network.On<AnnouncementEventData>([] (const MessageEnvelope<AnnouncementEventData>& envelope)
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
        m_network.Dispatch(request, callback);
    }

    void ChatOnlineService::SendMainRoomMessage(const MainRoomMessageRequest& request, const MessageCallback<MainRoomMessageRequest>& callback)
    {
        m_network.Dispatch(request, callback);
    }

    void ChatOnlineService::SendWaitingMessage(const WaitingMessageRequest& request, const MessageCallback<WaitingMessageRequest>& callback)
    {
        m_network.Dispatch(request, callback);
    }

    void ChatOnlineService::SendWhisper(
        const WhisperMessageRequest& request,
        const MessageCallback<WhisperMessageResponse>& callback
    )
    {
        m_network.Dispatch<WhisperMessageRequest, WhisperMessageResponse>(request, callback);
    }

    void ChatOnlineService::SetWhisperEventCallback(const MessageCallback<WhisperEventData>& callback)
    {
        m_whisperSubscriber = m_network.On<WhisperEventData>(callback);
    }

    void ChatOnlineService::SetMainRoomUserMessageCallback(const MessageCallback<MainRoomUserMessageResponse>& callback)
    {
        m_mainRoomUserSubscriber = m_network.On<MainRoomUserMessageResponse>(callback);
    }

    void ChatOnlineService::SetMainRoomAdminMessageCallback(const MessageCallback<MainRoomAdminMessageResponse>& callback)
    {
        m_mainRoomAdminSubscriber = m_network.On<MainRoomAdminMessageResponse>(callback);
    }

    void ChatOnlineService::SetWaitingUserMessageCallback(const MessageCallback<WaitingUserMessageResponse>& callback)
    {
        m_waitingUserSubscriber = m_network.On<WaitingUserMessageResponse>(callback);
    }

    void ChatOnlineService::SetWaitingAdminMessageCallback(const MessageCallback<WaitingAdminMessageResponse>& callback)
    {
        m_waitingAdminSubscriber = m_network.On<WaitingAdminMessageResponse>(callback);
    }
}
