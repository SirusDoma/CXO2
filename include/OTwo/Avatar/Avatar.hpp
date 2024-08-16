#ifndef O2JAM_AVATAR_AVATAR_HPP
#define O2JAM_AVATAR_AVATAR_HPP

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

#include <Genode/SceneGraph.hpp>

#include <map>

class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    Avatar();
    explicit Avatar(Gender gender);

    void Initialize() override;

    Gender GetGender() const;
    void SetGender(Gender gender);

    void SetDefaultItem(const Item *item);

    bool IsEquiped(const Item* item) const;
    void Equip(const Item* item, bool reset = true);
    void Unequip(const Item* item);
    void Unequip(EquipmentType type);
    void ClearEquipments();

    AvatarInfo* GetAvatarInfo() const;
    const Instrument& GetEquipedInstrumentType() const;
    const std::unordered_map<EquipmentType, const Item*> &GetEquipedItems() const;

    bool IsAlive() const;
    void Die();
    void Revive();


private:
    using ItemMap            = std::unordered_map<EquipmentType, const Item*>;
    using RenderableStateMap = std::unordered_map<const Gx::Updatable*, unsigned int>;

     static constexpr std::array<std::pair<EquipmentType, RenderPart>, 100> RenderLayerOrder = {
         {
            { EquipmentType::Costume,            RenderPart::Cape },
            { EquipmentType::Costume,            RenderPart::Body },
            { EquipmentType::Costume,            RenderPart::LeftArm },
            { EquipmentType::Costume,            RenderPart::RightArm },
            { EquipmentType::Accessories,        RenderPart::Cape },
            { EquipmentType::Accessories,        RenderPart::Body },
            { EquipmentType::Accessories,        RenderPart::LeftArm },
            { EquipmentType::Accessories,        RenderPart::RightArm },
            { EquipmentType::Wings,              RenderPart::Cape },
            { EquipmentType::Wings,              RenderPart::Body },
            { EquipmentType::Wings,              RenderPart::LeftArm },
            { EquipmentType::Wings,              RenderPart::RightArm },
            { EquipmentType::Body,               RenderPart::Cape },
            { EquipmentType::Body,               RenderPart::Body },
            { EquipmentType::Body,               RenderPart::LeftArm },
            { EquipmentType::Body,               RenderPart::RightArm },
            { EquipmentType::LeftArm,            RenderPart::Cape },
            { EquipmentType::LeftArm,            RenderPart::LeftArm },
            { EquipmentType::LeftArm,            RenderPart::RightArm },
            { EquipmentType::RightArm,           RenderPart::Cape },
            { EquipmentType::RightArm,           RenderPart::LeftArm },
            { EquipmentType::RightArm,           RenderPart::RightArm },
            { EquipmentType::Jacket,             RenderPart::Cape },
            { EquipmentType::Shoes,              RenderPart::Cape },
            { EquipmentType::Shoes,              RenderPart::Body },
            { EquipmentType::Shoes,              RenderPart::LeftArm },
            { EquipmentType::Shoes,              RenderPart::RightArm },
            { EquipmentType::Pants,              RenderPart::Cape },
            { EquipmentType::Pants,              RenderPart::Body },
            { EquipmentType::Pants,              RenderPart::LeftArm },
            { EquipmentType::Pants,              RenderPart::RightArm },
            { EquipmentType::Amulet,             RenderPart::Cape },
            { EquipmentType::Amulet,             RenderPart::Body },
            { EquipmentType::Amulet,             RenderPart::LeftArm },
            { EquipmentType::Amulet,             RenderPart::RightArm },
            { EquipmentType::Keyboard,           RenderPart::Cape },
            { EquipmentType::Keyboard,           RenderPart::Body },
            { EquipmentType::Keyboard,           RenderPart::LeftArm },
            { EquipmentType::Keyboard,           RenderPart::RightArm },
            { EquipmentType::LeftArm,            RenderPart::Body },
            { EquipmentType::Jacket,             RenderPart::Body },
            { EquipmentType::Jacket,             RenderPart::LeftArm },
            { EquipmentType::Hair,               RenderPart::Cape },
            { EquipmentType::Face,               RenderPart::Cape },
            { EquipmentType::Face,               RenderPart::Body },
            { EquipmentType::Face,               RenderPart::LeftArm },
            { EquipmentType::Face,               RenderPart::RightArm },
            { EquipmentType::Earrings,           RenderPart::Cape },
            { EquipmentType::Earrings,           RenderPart::Body },
            { EquipmentType::Earrings,           RenderPart::LeftArm },
            { EquipmentType::Earrings,           RenderPart::RightArm },
            { EquipmentType::Necklace,           RenderPart::Cape },
            { EquipmentType::Necklace,           RenderPart::Body },
            { EquipmentType::Necklace,           RenderPart::LeftArm },
            { EquipmentType::Necklace,           RenderPart::RightArm },
            { EquipmentType::Glasses,            RenderPart::Cape },
            { EquipmentType::Glasses,            RenderPart::Body },
            { EquipmentType::Glasses,            RenderPart::LeftArm },
            { EquipmentType::Glasses,            RenderPart::RightArm },
            { EquipmentType::Hair,               RenderPart::Body },
            { EquipmentType::Hair,               RenderPart::LeftArm },
            { EquipmentType::Hair,               RenderPart::RightArm },
            { EquipmentType::HairAccessories,    RenderPart::Cape },
            { EquipmentType::HairAccessories,    RenderPart::Body },
            { EquipmentType::HairAccessories,    RenderPart::LeftArm },
            { EquipmentType::HairAccessories,    RenderPart::RightArm },
            { EquipmentType::Bass,               RenderPart::Cape },
            { EquipmentType::Bass,               RenderPart::Body },
            { EquipmentType::Bass,               RenderPart::LeftArm },
            { EquipmentType::Bass,               RenderPart::RightArm },
            { EquipmentType::Guitar,             RenderPart::Cape },
            { EquipmentType::Guitar,             RenderPart::Body },
            { EquipmentType::Guitar,             RenderPart::LeftArm },
            { EquipmentType::Guitar,             RenderPart::RightArm },
            { EquipmentType::RightArm,           RenderPart::Body },
            { EquipmentType::Jacket,             RenderPart::RightArm },
            { EquipmentType::LeftHand,           RenderPart::Cape },
            { EquipmentType::LeftHand,           RenderPart::Body },
            { EquipmentType::LeftHand,           RenderPart::LeftArm },
            { EquipmentType::LeftHand,           RenderPart::RightArm },
            { EquipmentType::RightHand,          RenderPart::Cape },
            { EquipmentType::RightHand,          RenderPart::Body },
            { EquipmentType::RightHand,          RenderPart::LeftArm },
            { EquipmentType::RightHand,          RenderPart::RightArm },
            { EquipmentType::Glove,              RenderPart::Cape },
            { EquipmentType::Glove,              RenderPart::Body },
            { EquipmentType::Glove,              RenderPart::LeftArm },
            { EquipmentType::Glove,              RenderPart::RightArm },
            { EquipmentType::Drum,               RenderPart::Cape },
            { EquipmentType::Drum,               RenderPart::Body },
            { EquipmentType::Drum,               RenderPart::LeftArm },
            { EquipmentType::Drum,               RenderPart::RightArm },
            { EquipmentType::MusicalAccessories, RenderPart::Cape },
            { EquipmentType::MusicalAccessories, RenderPart::Body },
            { EquipmentType::MusicalAccessories, RenderPart::LeftArm },
            { EquipmentType::MusicalAccessories, RenderPart::RightArm },
            { EquipmentType::Pet,                RenderPart::Cape },
            { EquipmentType::Pet,                RenderPart::Body },
            { EquipmentType::Pet,                RenderPart::LeftArm },
            { EquipmentType::Pet,                RenderPart::RightArm },
        }
     };

    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    Gender     m_gender;
    Instrument m_instrument;
    ItemMap    m_items, m_defaultItems;
    bool       m_alive;

    mutable double m_elapsed;

    static unsigned int m_lastFrameID;
    static RenderableStateMap m_renderableStates;
};

#endif
