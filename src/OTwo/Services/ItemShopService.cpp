#include <OTwo/Services/ItemShopService.hpp>

#include <OTwo/Network/NetworkAdapter.hpp>

#include <OTwo/Messages/Requests/PurchaseItemRequest.hpp>
#include <OTwo/Messages/Requests/SellItemRequest.hpp>

#include <OTwo/Messages/Responses/PurchaseItemResponse.hpp>
#include <OTwo/Messages/Responses/SellItemResponse.hpp>

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
