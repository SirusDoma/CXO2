#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace O2CX
{
    class SubscriptionSet
    {
    public:
        template <typename TSubscriber>
        void Add(TSubscriber&& subscriber)
        {
            using Subscriber = std::decay_t<TSubscriber>;
            m_subscriptions.emplace_back(
                new Subscriber(std::forward<TSubscriber>(subscriber)),
                [] (void* subscription) { delete static_cast<Subscriber*>(subscription); }
            );
        }

        void Clear()
        {
            m_subscriptions.clear();
        }

    private:
        std::vector<std::unique_ptr<void, void(*)(void*)>> m_subscriptions;
    };
}
