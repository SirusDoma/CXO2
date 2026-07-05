#include <CXO2/Services/ItemShopService.hpp>
#include <CXO2/Services/MessageService.hpp>

#include <CXO2/Messages/Requests/PurchaseItemRequest.hpp>
#include <CXO2/Messages/Requests/SellItemRequest.hpp>

namespace Cx
{
    ItemShopOnlineService::ItemShopOnlineService(MessageService& messages) :
        m_messages(messages)
    {
    }

    void ItemShopOnlineService::PurchaseItem(
        const PurchaseItemRequest& request,
        const MessageCallback<PurchaseItemResponse>& callback
    ) const
    {
        m_messages.Dispatch<PurchaseItemRequest, PurchaseItemResponse>(request, callback);
    }

    void ItemShopOnlineService::SellItem(
        const SellItemRequest& request,
        const MessageCallback<SellItemResponse>& callback
    ) const
    {
        m_messages.Dispatch<SellItemRequest, SellItemResponse>(request, callback);
    }
}
