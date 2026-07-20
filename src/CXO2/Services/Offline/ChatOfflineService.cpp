#include <CXO2/Services/Offline/ChatOfflineService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/States/State.hpp>

namespace Cx
{
    ChatOfflineService::ChatOfflineService(SessionContext& session) :
        m_session(session)
    {
    }

    void ChatOfflineService::SendAnnouncement(const AnnouncementRequest& request, const MessageCallback<AnnouncementRequest>& callback)
    {
        State::Announce(request.Content);

        if (callback)
            callback(MessageEnvelope<AnnouncementRequest>(request));
    }

    void ChatOfflineService::SendMainRoomMessage(const MainRoomMessageRequest& request, const MessageCallback<MainRoomMessageRequest>& callback)
    {
        if (m_mainRoomUserCallback)
            m_mainRoomUserCallback(MessageEnvelope<MainRoomUserMessageResponse>(MainRoomUserMessageResponse{ m_session.GetName(), request.Content }));

        if (callback)
            callback(MessageEnvelope<MainRoomMessageRequest>(request));
    }

    void ChatOfflineService::SendWaitingMessage(const WaitingMessageRequest& request, const MessageCallback<WaitingMessageRequest>& callback)
    {
        if (m_waitingUserCallback)
            m_waitingUserCallback(MessageEnvelope<WaitingUserMessageResponse>(WaitingUserMessageResponse{ m_session.GetName(), request.Content }));

        if (callback)
            callback(MessageEnvelope<WaitingMessageRequest>(request));
    }

    void ChatOfflineService::SendWhisper(
        const WhisperMessageRequest& request,
        const MessageCallback<WhisperMessageResponse>& callback
    )
    {
        if (m_whisperCallback)
            m_whisperCallback(MessageEnvelope<WhisperEventData>(WhisperEventData{ request.Recipient, request.Content }));

        if (callback)
            callback(MessageEnvelope<WhisperMessageResponse>(WhisperMessageResponse{ 0, request.Recipient, request.Content }));
    }

    void ChatOfflineService::SetWhisperEventCallback(const MessageCallback<WhisperEventData>& callback)
    {
        m_whisperCallback = callback;
    }

    void ChatOfflineService::SetMainRoomUserMessageCallback(const MessageCallback<MainRoomUserMessageResponse>& callback)
    {
        m_mainRoomUserCallback = callback;
    }

    void ChatOfflineService::SetMainRoomAdminMessageCallback(const MessageCallback<MainRoomAdminMessageResponse>& callback)
    {
        m_mainRoomAdminCallback = callback;
    }

    void ChatOfflineService::SetWaitingUserMessageCallback(const MessageCallback<WaitingUserMessageResponse>& callback)
    {
        m_waitingUserCallback = callback;
    }

    void ChatOfflineService::SetWaitingAdminMessageCallback(const MessageCallback<WaitingAdminMessageResponse>& callback)
    {
        m_waitingAdminCallback = callback;
    }
}
