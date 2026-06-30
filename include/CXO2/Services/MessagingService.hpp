#pragma once

#include <CXO2/Services/EventService.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct CharacterInfo;
    class NetworkAdapter;
    class SessionContext;
    class MessagingService : public virtual Service
    {
    public:
        virtual void SendAnnouncement(
            const sf::String& message,
            std::function<void()> callback = nullptr
        ) = 0;

        virtual void SendMessage(
            const sf::String& message,
            std::function<void()> callback = nullptr
        ) = 0;

        virtual void UnsubscribeEvents() = 0;

        virtual void SendWhisper(
            const sf::String& recipient,
            const sf::String& message,
            std::function<void(bool)> callback = nullptr
        ) = 0;

        virtual void OnMessageReceive(
            std::function<void(const CharacterInfo&, const sf::String&, bool)> callback
        ) = 0;
    };

    class MessagingOnlineService : public MessagingService, public EventService
    {
    public:
        explicit MessagingOnlineService(NetworkAdapter& adapter);

        void SendAnnouncement(
            const sf::String& message,
            std::function<void()> callback = nullptr
        ) override;

        void SendMessage(
            const sf::String& message,
            std::function<void()> callback = nullptr
        ) override;

        void SendWhisper(
            const sf::String& recipient,
            const sf::String& message,
            std::function<void(bool)> callback = nullptr
        ) override;

        void OnMessageReceive(
            std::function<void(const CharacterInfo&, const sf::String&, bool)> callback
        ) override;

        void UnsubscribeEvents() override;
    };

    class MessagingOfflineService : public MessagingService
    {
    };
}
