#pragma once

#include <CXO2/Services/ItemShopService.hpp>

namespace Cx
{
    class NetworkService;
    class ItemShopOnlineService : public ItemShopService
    {
    public:
        explicit ItemShopOnlineService(NetworkService& network);

        void PurchaseItem(
            const PurchaseItemRequest& request,
            const MessageCallback<PurchaseItemResponse>& callback = nullptr
        ) const override;

        void SellItem(
            const SellItemRequest& request,
            const MessageCallback<SellItemResponse>& callback = nullptr
        ) const override;

    private:
        NetworkService& m_network;
    };
}
