#ifndef O2JAM_CHARACTER_AVATAR_HPP
#define O2JAM_CHARACTER_AVATAR_HPP

#include <Genode/SceneGraph.hpp>
#include <O2/Character/Item.hpp>

#include <O2/Data/Room.hpp>

#include <map>


class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    Avatar(Room::PlayerInfo playerInfo);
    const Room::PlayerInfo &GetPlayerInfo() const;
    void Equip(Item* item);

private:
    constexpr static const Equipment::Type TYPE_RENDER_ORDER[] = {
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

    constexpr static const Equipment::RenderType PART_RENDER_ORDER[] = {
        Equipment::RenderType::Body,
        Equipment::RenderType::LeftArm,
        Equipment::RenderType::RightArm,
        Equipment::RenderType::Head
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

    Room::PlayerInfo                 m_playerInfo;
    Equipment::Instrument            m_instrument;
    std::map<Equipment::Type, Item*> m_items;
};

#endif
