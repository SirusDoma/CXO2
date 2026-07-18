#pragma once

#include <CXO2/Services/ItemShopService.hpp>

namespace Cx
{
    class SessionContext;
    class ItemShopOfflineService : public ItemShopService
    {
    public:
        explicit ItemShopOfflineService(SessionContext& session);

        void PurchaseItem(
            const PurchaseItemRequest& request,
            const MessageCallback<PurchaseItemResponse>& callback = nullptr
        ) const override;

        void SellItem(
            const SellItemRequest& request,
            const MessageCallback<SellItemResponse>& callback = nullptr
        ) const override;

    private:
        SessionContext& m_session;
    };
}
