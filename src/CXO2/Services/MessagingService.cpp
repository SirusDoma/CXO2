#include <CXO2/Services/MessagingService.hpp>

#include <CXO2/Models/Character.hpp>

#include <CXO2/Messages/Requests/AnnouncementRequest.hpp>
#include <CXO2/Messages/Requests/MainRoomMessageRequest.hpp>
#include <CXO2/Messages/Requests/WaitingMessageRequest.hpp>
#include <CXO2/Messages/Requests/WhisperMessageRequest.hpp>

#include <CXO2/Messages/Responses/MainRoomAdminMessageResponse.hpp>
#include <CXO2/Messages/Responses/MainRoomUserMessageResponse.hpp>
#include <CXO2/Messages/Responses/WaitingAdminMessageResponse.hpp>
#include <CXO2/Messages/Responses/WaitingUserMessageResponse.hpp>
#include <CXO2/Messages/Responses/WhisperMessageResponse.hpp>

#include <CXO2/Messages/Events/WhisperEventData.hpp>

#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/States/StateWaiting7K.hpp>

#include <Genode/SceneGraph/SceneDirector.hpp>

namespace Cx
{
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
}
