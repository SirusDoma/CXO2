#include <OTwo/Services/EventService.hpp>
#include <SFML/System/Sleep.hpp>

EventService::EventService(NetworkAdapter& adapter) :
    m_adapter(adapter)
{
}

EventService::~EventService()
{
}

NetworkAdapter& EventService::GetNetworkAdapter() const
{
    return m_adapter;
}
