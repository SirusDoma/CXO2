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
}
