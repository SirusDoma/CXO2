#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Models/Room.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/EquipItemResponse.hpp>
#include <CXO2/Network/Responses/SellItemResponse.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <CXO2/UI/Image.hpp>

#include <cstddef>
#include <unordered_map>

namespace Cx
{
    class Control;

    class ItemShopService;
    class ItemFactory;
    class CharacterService;
    class SessionContext;
    class StateMyRoom : public State
    {
    public:
        StateMyRoom(Gx::AudioMixer& mixer, SessionContext& session, CharacterService& service, ItemShopService& shopService, ItemFactory& items);

        void Initialize() override;

    private:
        void Invalidate();
        void InvalidateSlot(Image* slot, EquipmentType type, RenderPart thumbnail = RenderPart::LargeThumbnail);

        void OnBagSlotClicked(Control& sender, Control::Event& ev);
        void OnBagSlotDoubleClicked(Control& sender, Control::Event& ev);
        void OnEquippedSlotDoubleClicked(Control& sender, Control::Event& ev);

        void OnEquipItemResponded(const MessageEnvelope<EquipItemResponse>& ev);
        void OnUnequipItemResponded(const MessageEnvelope<EquipItemResponse>& ev, const Item& item);
        void OnSellItemResponded(const MessageEnvelope<SellItemResponse>& ev);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        CharacterService& m_service;
        ItemShopService& m_shopService;
        ItemFactory& m_items;

        unsigned int m_bagCurrentPage = 0;
        unsigned int m_bagMaxPage     = 0;

        bool m_busy = false;
        Item* m_selectedItem;
        Image* m_bagSelectIndicator;
        std::vector<Item> m_inventory;

        std::unordered_map<Control*, Item*> m_bagSlotItems;
        std::unordered_map<Control*, unsigned int> m_bagSlotQuantities;
        std::unordered_map<Control*, std::size_t> m_bagSlotTargets;
        std::unordered_map<Control*, const Item*> m_equippedSlotItems;
    };
}
