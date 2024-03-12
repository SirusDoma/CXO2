#ifndef O2JAM_AVATAR_AVATAR_HPP
#define O2JAM_AVATAR_AVATAR_HPP

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Data/Equipment.hpp>
#include <OTwo/Data/Room.hpp>

#include <Genode/SceneGraph.hpp>

#include <map>


class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    Avatar();
    Avatar(Player playerInfo);

    const Player &GetPlayer() const;

    void SetPlayer(const Player& playerInfo);
    void SetDefaultItem(const Item *item);

    bool IsEquiped(const Item* item) const;
    void Equip(const Item* item);
    void Unequip(const Item* item);

    const std::map<EquipmentType, const Item*> &GetEquipedItems() const;
    const Instrument &GetEquipedInstrumentType() const;

private:
    constexpr static const EquipmentType RENDER_TYPE_ORDER[] = {
        EquipmentType::Wings,
        EquipmentType::Body,
        EquipmentType::LeftArm,
        EquipmentType::RightArm,
        EquipmentType::Jacket,
        EquipmentType::Shoes,
        EquipmentType::Pants,
        EquipmentType::Piano,
        EquipmentType::Bass,
        EquipmentType::Drum,
        EquipmentType::LeftHand,
        EquipmentType::RightHand,
        EquipmentType::Armlet,
        EquipmentType::Accessories,
        EquipmentType::Glove,
        EquipmentType::Guitar,
        EquipmentType::Face,
        EquipmentType::Earrings,
        EquipmentType::Necklace,
        EquipmentType::Glasses,
        EquipmentType::Hair,
        EquipmentType::HairAccessories,
        EquipmentType::MusicalAccessories,
        EquipmentType::Pet,
    };

    constexpr static const RenderPart RENDER_PART_ORDER[] = {
        RenderPart::Cape,
        RenderPart::Body,
        RenderPart::LeftArm,
        RenderPart::RightArm,
    };

    constexpr static const Instrument INSTRUMENT_RENDER_ORDER[] = {
        Instrument::None,
        Instrument::Bass,
        Instrument::Guitar,
        Instrument::Piano,
        Instrument::Drum
    };

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    Player                      m_player;
    Instrument                  m_instrument;
    std::map<EquipmentType, const Item*> m_items, m_defaultItems;
};

#endif
