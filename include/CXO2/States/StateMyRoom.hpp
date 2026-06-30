#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Models/Room.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/Image.hpp>

namespace Cx
{
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
        void InvalidateSlot(Gx::Image* slot, EquipmentType type, RenderPart thumbnail = RenderPart::LargeThumbnail);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        CharacterService& m_service;
        ItemShopService& m_shopService;
        ItemFactory& m_items;

        unsigned int m_bagCurrentPage = 0;
        unsigned int m_bagMaxPage     = 0;

        bool m_busy = false;
        Item* m_selectedItem;
        Gx::Image* m_bagSelectIndicator;
        std::vector<Item> m_inventory;
    };
}
