#ifndef O2JAM_CHARACTER_AVATAR_HPP
#define O2JAM_CHARACTER_AVATAR_HPP

#include <Genode/SceneGraph.hpp>
#include <O2/Character/Item.hpp>

#include <O2/Data/Room.hpp>

#include <map>


class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    Avatar();
    Avatar(Room::PlayerInfo playerInfo);

    const Room::PlayerInfo &GetPlayerInfo() const;

    void SetPlayerInfo(const Room::PlayerInfo& playerInfo);
    void SetDefaultItem(const Item *item);

    bool IsEquiped(const Item* item) const;
    void Equip(const Item* item);
    void Unequip(const Item* item);

    const std::map<Equipment::Type, const Item*> &GetEquipedItems() const;
    const Equipment::Instrument &GetEquipedInstrumentType() const;

private:
    constexpr static const Equipment::Type RENDER_TYPE_ORDER[] = {
        Equipment::Type::Wings,
        Equipment::Type::Body,
        Equipment::Type::LeftArm,
        Equipment::Type::RightArm,
        Equipment::Type::Jacket,
        Equipment::Type::Shoes,
        Equipment::Type::Pants,
        Equipment::Type::Piano,
        Equipment::Type::Bass,
        Equipment::Type::Drum,
        Equipment::Type::LeftHand,
        Equipment::Type::RightHand,
        Equipment::Type::Armlet,
        Equipment::Type::Accessories,
        Equipment::Type::Glove,
        Equipment::Type::Guitar,
        Equipment::Type::Face,
        Equipment::Type::Earrings,
        Equipment::Type::Necklace,
        Equipment::Type::Glasses,
        Equipment::Type::Hair,
        Equipment::Type::HairAccessories,
        Equipment::Type::MusicalAccessories,
        Equipment::Type::Pet,
    };

    constexpr static const Equipment::RenderPart RENDER_PART_ORDER[] = {
        Equipment::RenderPart::Cape,
        Equipment::RenderPart::Body,
        Equipment::RenderPart::LeftArm,
        Equipment::RenderPart::RightArm,
    };

    constexpr static const Equipment::Instrument INSTRUMENT_RENDER_ORDER[] = {
        Equipment::Instrument::None,
        Equipment::Instrument::Bass,
        Equipment::Instrument::Guitar,
        Equipment::Instrument::Piano,
        Equipment::Instrument::Drum
    };

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    Room::PlayerInfo                       m_playerInfo;
    Equipment::Instrument                  m_instrument;
    std::map<Equipment::Type, const Item*> m_items, m_defaultItems;
};

#endif
