#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Models/Character.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <SFML/System/String.hpp>

#include <utility>

namespace Cx
{
    class ChatPanel;

    enum class ChatEventKey
    {
        OnInitialize,
        OnSend,
        OnMessage,
        OnWhisper
    };

    enum class ChatSendType
    {
        Whisper,
        Announcement,
        Waiting,
        MainRoom
    };

    struct ChatEventArgs : Interceptable {};

    struct ChatSendEventArgs : Interceptable
    {
        const ChatSendType Type;
        sf::String Message;
        sf::String Recipient;

        ChatSendEventArgs(const ChatSendType type, sf::String message, sf::String recipient) :
            Type(type),
            Message(std::move(message)),
            Recipient(std::move(recipient))
        {
        }
    };

    struct ChatMessageEventArgs : Interceptable
    {
        const Role SenderRole;
        sf::String Sender;
        sf::String Message;

        ChatMessageEventArgs(const Role senderRole, sf::String sender, sf::String message) :
            SenderRole(senderRole),
            Sender(std::move(sender)),
            Message(std::move(message))
        {
        }
    };

    struct ChatWhisperEventArgs : Interceptable
    {
        sf::String Sender;
        sf::String Message;

        ChatWhisperEventArgs(sf::String sender, sf::String message) :
            Sender(std::move(sender)),
            Message(std::move(message))
        {
        }
    };

    using ChatEvent        = Gx::Event<ChatEventKey, ChatPanel, ChatEventArgs>;
    using ChatSendEvent    = Gx::Event<ChatEventKey, ChatPanel, ChatSendEventArgs>;
    using ChatMessageEvent = Gx::Event<ChatEventKey, ChatPanel, ChatMessageEventArgs>;
    using ChatWhisperEvent = Gx::Event<ChatEventKey, ChatPanel, ChatWhisperEventArgs>;

    namespace ChatEvents
    {
        inline constexpr ChatEvent        OnInitialize{ChatEventKey::OnInitialize};
        inline constexpr ChatSendEvent    OnSend{ChatEventKey::OnSend};
        inline constexpr ChatMessageEvent OnMessage{ChatEventKey::OnMessage};
        inline constexpr ChatWhisperEvent OnWhisper{ChatEventKey::OnWhisper};
    }
}
