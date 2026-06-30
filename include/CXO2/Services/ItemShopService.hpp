#pragma once

#include <CXO2/Services/Service.hpp>

#include <cstdint>
#include <functional>

namespace Cx
{
    struct PurchaseItemResponse;
    struct SellItemResponse;

    class NetworkAdapter;
    class NetworkException;
    class ItemShopService : public Service
    {
    public:
        virtual void PurchaseItem(
            std::uint32_t itemID,
            std::function<void(const PurchaseItemResponse&)> callback,
            std::function<void(const NetworkException& ex)> errorCallback = nullptr
        ) const = 0;

        virtual void SellItem(
           std::uint32_t slotID,
           std::function<void(const SellItemResponse&)> callback,
           std::function<void(const NetworkException& ex)> errorCallback = nullptr
       ) const = 0;
    };

    class ItemShopOnlineService : public ItemShopService
    {
    public:
        explicit ItemShopOnlineService(NetworkAdapter& adapter);

        void PurchaseItem(
            std::uint32_t itemID,
            std::function<void(const PurchaseItemResponse&)> callback,
            std::function<void(const NetworkException& ex)> errorCallback = nullptr
        ) const override;

        void SellItem(
            std::uint32_t slotID,
            std::function<void(const SellItemResponse&)> callback,
            std::function<void(const NetworkException& ex)> errorCallback = nullptr
        ) const override;

    private:
        NetworkAdapter& m_adapter;
    };

    class ItemShopOfflineService : public ItemShopService
    {
        void PurchaseItem(
            std::uint32_t itemID,
            std::function<void(const PurchaseItemResponse&)> callback,
            std::function<void(const NetworkException& ex)> errorCallback = nullptr
        ) const override;

        void SellItem(
            std::uint32_t slotID,
            std::function<void(const SellItemResponse&)> callback,
            std::function<void(const NetworkException& ex)> errorCallback = nullptr
        ) const override;
    };
}
