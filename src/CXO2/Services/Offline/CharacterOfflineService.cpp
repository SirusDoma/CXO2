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
        auto response = CharacterInfoResponse{};
        response.Suspended = 0;
        if (m_session.GetName().isEmpty())
        {
            response.Name   = sf::String("Player");
            response.Gender = Gender::Male;
            response.Level  = 1;
            response.Gem    = 100000;
            response.Point  = 100000;
        }
        else
        {
            response.Name       = m_session.GetName();
            response.Gender     = m_session.GetGender();
            response.Gem        = m_session.GetWallet().Gem;
            response.Point      = m_session.GetWallet().Cash;
            response.Level      = m_session.GetLevel();
            response.Wins       = m_session.GetRankStats().Wins;
            response.Loses      = m_session.GetRankStats().Loses;
            response.Draws      = m_session.GetRankStats().Draws;
            response.Experience = m_session.GetExperience();
            response.Role       = m_session.GetRole();
        }

        response.EquippedItemIDs = m_session.GetEquippedItemIDs();

        auto inventory = std::vector<std::uint32_t>();
        for (const auto& item : m_session.GetInventory())
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
        const auto& inventory = m_session.GetInventory();

        auto response = EquipItemResponse{};
        response.EquipSlot = request.EquipSlot;
        response.SlotID    = request.SlotID;

        if (request.SlotID >= inventory.size())
            response.Invalid = 1;
        else
        {
            response.NewEquippedItemId = inventory[request.SlotID].ID;

            for (const auto id : m_session.GetEquippedItemIDs())
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
