#include <CXO2/Services/Offline/CharacterOfflineService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/Network/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Network/Responses/EquipItemResponse.hpp>

namespace Cx
{
    namespace
    {
        constexpr std::size_t InventorySlotCount = 30;

        ItemEquipSlotType GetItemEquipSlotType(const EquipmentType type)
        {
            switch (type)
            {
                case EquipmentType::Guitar:             return ItemEquipSlotType::Instrument;
                case EquipmentType::Bass:               return ItemEquipSlotType::Instrument;
                case EquipmentType::Keyboard:           return ItemEquipSlotType::Instrument;
                case EquipmentType::Drum:               return ItemEquipSlotType::Instrument;
                case EquipmentType::Hair:               return ItemEquipSlotType::Hair;
                case EquipmentType::Earrings:           return ItemEquipSlotType::Earrings;
                case EquipmentType::Gloves:             return ItemEquipSlotType::Gloves;
                case EquipmentType::Accessories:        return ItemEquipSlotType::Accessories;
                case EquipmentType::Top:                return ItemEquipSlotType::Top;
                case EquipmentType::Pants:              return ItemEquipSlotType::Pants;
                case EquipmentType::Glasses:            return ItemEquipSlotType::Glasses;
                case EquipmentType::Necklace:           return ItemEquipSlotType::Necklace;
                case EquipmentType::ClothesAccessories: return ItemEquipSlotType::ClothesAccessories;
                case EquipmentType::Shoes:              return ItemEquipSlotType::Shoes;
                case EquipmentType::Face:               return ItemEquipSlotType::Face;
                default:                                return ItemEquipSlotType{};
            }
        }
    }

    CharacterOfflineService::CharacterOfflineService(SessionContext& session, ItemFactory& items) :
        m_session(session),
        m_items(items)
    {
    }

    void CharacterOfflineService::GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const
    {
        const auto& info = m_session.GetCharacterInfo();

        auto response = CharacterInfoResponse{};
        response.Suspended = 0;
        if (info.Name.isEmpty())
        {
            response.Name   = sf::String("Player");
            response.Gender = Gender::Male;
            response.Level  = 1;
            response.Gem    = 100000;
            response.Point  = 100000;
        }
        else
        {
            response.Name       = info.Name;
            response.Gender     = info.Gender;
            response.Gem        = info.Wallet.Gem;
            response.Point      = info.Wallet.Cash;
            response.Level      = info.Level;
            response.Wins       = info.RankStats.Wins;
            response.Loses      = info.RankStats.Loses;
            response.Draws      = info.RankStats.Draws;
            response.Experience = info.Experience;
            response.Role       = info.Role;
        }

        response.EquippedItemIDs = info.EquippedItemIDs;

        auto inventory = std::vector<std::uint32_t>();
        for (const auto& item : info.Inventory)
        {
            if (item.ID == 0)
                continue;

            for (std::uint32_t i = 0; i < item.Quantity && inventory.size() < InventorySlotCount; i++)
                inventory.push_back(item.ID);
        }
        inventory.resize(InventorySlotCount, 0);
        response.Inventory = inventory;

        if (callback)
            callback(MessageEnvelope<CharacterInfoResponse>(response));
    }

    void CharacterOfflineService::Equip(
        const EquipItemRequest& request,
        const MessageCallback<EquipItemResponse>& callback
    ) const
    {
        const auto& info = m_session.GetCharacterInfo();

        auto response = EquipItemResponse{};
        response.EquipSlot = request.EquipSlot;
        response.SlotID    = request.SlotID;

        if (request.SlotID >= info.Inventory.size())
            response.Invalid = 1;
        else
        {
            response.NewEquippedItemId = info.Inventory[request.SlotID].ID;

            for (const auto id : info.EquippedItemIDs)
            {
                const auto item = m_items.Create(id);
                if (GetItemEquipSlotType(item.GetType()) == request.EquipSlot)
                {
                    response.PreviousEquippedItemId = id;
                    break;
                }
            }
        }

        if (callback)
            callback(MessageEnvelope<EquipItemResponse>(response));

        m_session.Save();
    }
}
