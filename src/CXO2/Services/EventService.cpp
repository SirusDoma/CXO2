#include <CXO2/Services/EventService.hpp>
#include <SFML/System/Sleep.hpp>

namespace Cx
{
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
}
