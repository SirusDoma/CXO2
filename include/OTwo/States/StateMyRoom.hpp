#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Room.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/Image.hpp>

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
