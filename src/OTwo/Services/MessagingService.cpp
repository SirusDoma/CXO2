#include <OTwo/Services/MessagingService.hpp>

#include <OTwo/Models/Character.hpp>

#include <OTwo/Messages/Requests/AnnouncementRequest.hpp>
#include <OTwo/Messages/Requests/MainRoomMessageRequest.hpp>
#include <OTwo/Messages/Requests/WaitingMessageRequest.hpp>
#include <OTwo/Messages/Requests/WhisperMessageRequest.hpp>

#include <OTwo/Messages/Responses/MainRoomAdminMessageResponse.hpp>
#include <OTwo/Messages/Responses/MainRoomUserMessageResponse.hpp>
#include <OTwo/Messages/Responses/WaitingAdminMessageResponse.hpp>
#include <OTwo/Messages/Responses/WaitingUserMessageResponse.hpp>
#include <OTwo/Messages/Responses/WhisperMessageResponse.hpp>

#include <OTwo/Messages/Events/WhisperEventData.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/States/StateWaiting7K.hpp>

#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Application.hpp>

MessagingOnlineService::MessagingOnlineService(NetworkAdapter& adapter) :
    EventService(adapter)
{
}

void MessagingOnlineService::SendAnnouncement(const sf::String& message, std::function<void()> callback)
{
    GetNetworkAdapter().SendAsync(AnnouncementRequest{message}, callback);
}

void MessagingOnlineService::SendMessage(const sf::String& message, const std::function<void()> callback)
{
    const auto& director = Gx::Application::Instance().GetSceneDirector();
    if (!director.IsPresenting<StateWaiting7K>() && !director.IsPresenting<StatePlaying7K>())
        GetNetworkAdapter().SendAsync(MainRoomMessageRequest{message}, callback);
    else
        GetNetworkAdapter().SendAsync(WaitingMessageRequest{message}, callback);
}

void MessagingOnlineService::SendWhisper(
    const sf::String& recipient,
    const sf::String& message,
    const std::function<void(bool)> callback
)
{
    GetNetworkAdapter().Exchange<WhisperMessageRequest, WhisperMessageResponse>(
        WhisperMessageRequest{ recipient, message },
        [callback] (const auto& response)
        {
            if (callback)
                callback(!response.Invalid);
        }
    );
}

void MessagingOnlineService::OnMessageReceive(std::function<void(const CharacterInfo&, const sf::String&, bool)> callback)
{
    Subscribe<WhisperEventData>([callback] (const auto& response)
    {
        if (callback)
        {
            callback(
                CharacterInfo{
                    response.Sender,
                    Gender::Any,
                    Role::Normal
                },
                response.Content,
                true
            );
        }
    });

    const auto& director = Gx::Application::Instance().GetSceneDirector();
    if (!director.IsPresenting<StateWaiting7K>() && !director.IsPresenting<StatePlaying7K>())
    {
        Subscribe<MainRoomUserMessageResponse>([callback] (const auto& response)
        {
            if (callback)
            {
                callback(
                    CharacterInfo{
                        response.Sender,
                        Gender::Any,
                        Role::Normal
                    },
                    response.Content,
                    false
                );
            }
        });

        Subscribe<MainRoomAdminMessageResponse>([callback] (const auto& response)
        {
            if (callback)
            {
                callback(
                    CharacterInfo{
                        response.Sender,
                        Gender::Any,
                        Role::Administrator
                    },
                    response.Content,
                    false
                );
            }
        });
    }
    else
    {
        Subscribe<WaitingUserMessageResponse>([callback] (const auto& response)
        {
            if (callback)
            {
                callback(
                    CharacterInfo{
                        response.Sender,
                        Gender::Any,
                        Role::Normal
                    },
                    response.Content,
                    false
                );
            }
        });

        Subscribe<WaitingAdminMessageResponse>([callback] (const auto& response)
        {
            if (callback)
            {
                callback(
                    CharacterInfo{
                        response.Sender,
                        Gender::Any,
                        Role::Administrator
                    },
                    response.Content,
                    false
                );
            }
        });
    }
}

void MessagingOnlineService::UnsubscribeEvents()
{
    Unsubscribe<WhisperEventData>();
    Unsubscribe<MainRoomUserMessageResponse>();
    Unsubscribe<MainRoomAdminMessageResponse>();
    Unsubscribe<WaitingUserMessageResponse>();
    Unsubscribe<WaitingAdminMessageResponse>();
}
