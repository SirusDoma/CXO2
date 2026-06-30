#include <CXO2/Services/ItemShopService.hpp>

#include <CXO2/Network/NetworkAdapter.hpp>

#include <CXO2/Messages/Requests/PurchaseItemRequest.hpp>
#include <CXO2/Messages/Requests/SellItemRequest.hpp>

#include <CXO2/Messages/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Messages/Responses/SellItemResponse.hpp>

namespace Cx
{
    ItemShopOnlineService::ItemShopOnlineService(NetworkAdapter& adapter) :
        m_adapter(adapter)
    {
    }

    void ItemShopOnlineService::PurchaseItem(
        const std::uint32_t itemID,
        std::function<void(const PurchaseItemResponse&)> callback,
        const std::function<void(const NetworkException& ex)> errorCallback
    ) const
    {
        m_adapter.Exchange<PurchaseItemRequest, PurchaseItemResponse>(
            PurchaseItemRequest{itemID},
            [=] (const auto& response)
            {
                if (callback)
                    callback(response);
            },
            errorCallback
        );
    }

    void ItemShopOnlineService::SellItem(
        const std::uint32_t slotID,
        const std::function<void(const SellItemResponse&)> callback,
        const std::function<void(const NetworkException& ex)> errorCallback) const
    {
        m_adapter.Exchange<SellItemRequest, SellItemResponse>(
            SellItemRequest{slotID},
            callback,
            errorCallback
        );
    }
}
