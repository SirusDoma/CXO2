#include <CXO2/Services/Offline/ItemShopOfflineService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Network/Requests/PurchaseItemRequest.hpp>
#include <CXO2/Network/Requests/SellItemRequest.hpp>

#include <algorithm>

namespace Cx
{
    ItemShopOfflineService::ItemShopOfflineService(SessionContext& session) :
        m_session(session)
    {
    }

    void ItemShopOfflineService::PurchaseItem(
        const PurchaseItemRequest& request,
        const MessageCallback<PurchaseItemResponse>& callback
    ) const
    {
        const auto& inventory = m_session.GetInventory();

        auto response = PurchaseItemResponse{};
        response.Gem    = m_session.GetWallet().Gem;
        response.Cash   = m_session.GetWallet().Cash;
        response.ItemID = request.ItemID;

        const auto it = std::find_if(inventory.begin(), inventory.end(), [] (const auto& item)
        {
            return item.ID == 0;
        });

        if (it == inventory.end())
            response.ResultCode = PurchaseItemResult::InventoryFull;
        else
        {
            response.ResultCode = PurchaseItemResult::Success;
            response.SlotID     = static_cast<std::uint32_t>(std::distance(inventory.begin(), it));
        }

        if (callback)
            callback(MessageEnvelope<PurchaseItemResponse>(response));

        m_session.Save();
    }

    void ItemShopOfflineService::SellItem(
        const SellItemRequest& request,
        const MessageCallback<SellItemResponse>& callback
    ) const
    {
        const auto& inventory = m_session.GetInventory();

        auto response = SellItemResponse{};
        response.Gem    = m_session.GetWallet().Gem;
        response.Cash   = m_session.GetWallet().Cash;
        response.SlotID = request.SlotID;

        if (request.SlotID < inventory.size() && inventory[request.SlotID].ID != 0)
            response.Result = SellItemResult::Success;
        else
            response.Result = SellItemResult::Failed;

        if (callback)
            callback(MessageEnvelope<SellItemResponse>(response));

        m_session.Save();
    }
}
