#pragma once

#include <OTwo/Services/Service.hpp>
#include <OTwo/Messages/Commands.hpp>

#include <type_traits>
#include <functional>

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

#include <OTwo/Services/EventService.inl>
