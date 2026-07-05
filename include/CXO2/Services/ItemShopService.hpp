#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Messages/MessageEnvelope.hpp>

#include <CXO2/Messages/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Messages/Responses/SellItemResponse.hpp>

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

    class MessageService;
    class ItemShopOnlineService : public ItemShopService
    {
    public:
        explicit ItemShopOnlineService(MessageService& messages);

        void PurchaseItem(
            const PurchaseItemRequest& request,
            const MessageCallback<PurchaseItemResponse>& callback = nullptr
        ) const override;

        void SellItem(
            const SellItemRequest& request,
            const MessageCallback<SellItemResponse>& callback = nullptr
        ) const override;

    private:
        MessageService& m_messages;
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
