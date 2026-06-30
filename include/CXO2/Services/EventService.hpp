#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Messages/Commands.hpp>

#include <type_traits>
#include <functional>

namespace Cx
{
    class NetworkAdapter;
    class EventService : public virtual Service
    {
    public:
        ~EventService() override;

        template<typename TMessage>
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Subscribe(std::function<void(const TMessage&)> subscriber);

        template<typename TMessage>
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Unsubscribe();

    protected:
        explicit EventService(NetworkAdapter& adapter);
        NetworkAdapter& GetNetworkAdapter() const;

    private:
        NetworkAdapter& m_adapter;
    };

}

#include <CXO2/Services/EventService.inl>
