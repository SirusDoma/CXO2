#include <CXO2/Services/Online/ItemShopOnlineService.hpp>
#include <CXO2/Services/NetworkService.hpp>

#include <CXO2/Network/Requests/PurchaseItemRequest.hpp>
#include <CXO2/Network/Requests/SellItemRequest.hpp>

namespace Cx
{
    ItemShopOnlineService::ItemShopOnlineService(NetworkService& network) :
        m_network(network)
    {
    }

    void ItemShopOnlineService::PurchaseItem(
        const PurchaseItemRequest& request,
        const MessageCallback<PurchaseItemResponse>& callback
    ) const
    {
        m_network.Dispatch<PurchaseItemRequest, PurchaseItemResponse>(request, callback);
    }

    void ItemShopOnlineService::SellItem(
        const SellItemRequest& request,
        const MessageCallback<SellItemResponse>& callback
    ) const
    {
        m_network.Dispatch<SellItemRequest, SellItemResponse>(request, callback);
    }
}
