#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>

#include <CXO2/Network/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Network/Responses/SellItemResponse.hpp>

#include <functional>

namespace Cx
{
    struct PurchaseItemRequest;
    struct SellItemRequest;

    class ItemShopService : public Service
    {
    public:
        virtual void PurchaseItem(
            const PurchaseItemRequest& request,
            const MessageCallback<PurchaseItemResponse>& callback = nullptr
        ) const = 0;

        virtual void SellItem(
            const SellItemRequest& request,
            const MessageCallback<SellItemResponse>& callback = nullptr
        ) const = 0;
    };

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

    class ItemShopOfflineService : public ItemShopService
    {
        void PurchaseItem(
            const PurchaseItemRequest& request,
            const MessageCallback<PurchaseItemResponse>& callback = nullptr
        ) const override;

        void SellItem(
            const SellItemRequest& request,
            const MessageCallback<SellItemResponse>& callback = nullptr
        ) const override;
    };
}
