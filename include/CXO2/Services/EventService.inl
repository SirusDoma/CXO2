#pragma once
#include <CXO2/Network/NetworkAdapter.hpp>

namespace Cx
{
    template<typename TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void> EventService::Subscribe(
        std::function<void(const TMessage&)> subscriber)
    {
        m_adapter.Subscribe<TMessage>([this, subscriber] (const auto& response)
        {
            if (subscriber)
                subscriber(response);
        },
        [] (const auto&)
        {
            // Log
        });
    }


    template<typename TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void> EventService::Unsubscribe()
    {
        m_adapter.Unsubscribe(TMessage::Command);
    }
}
