#pragma once

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>

#include <Genode/SceneGraph.hpp>

#include <map>

class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    using ItemMap = std::unordered_map<EquipmentType, Item>;

    Avatar();
    explicit Avatar(Gender gender);

    void Initialize() override;

    Gender GetGender() const;
    void SetGender(Gender gender);

    void SetDefaultItem(const Item& item);

    bool IsEquiped(const Item& item) const;
    void Equip(const Item& item);
    void Unequip(const Item& item);
    void Unequip(EquipmentType type);
    void ClearEquipments();

    AvatarInfo* GetAvatarInfo() const;
    const Instrument& GetEquipedInstrumentType() const;
    std::unordered_map<EquipmentType, const Item*> GetEquipedItems(bool includeDefaultItems = false) const;

    bool IsAlive() const;
    void Die();
    void Revive();

    void ResetRenderables() const;

private:
    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    Gender     m_gender;
    Instrument m_instrument;
    bool       m_alive;
    mutable ItemMap m_items, m_defaultItems;
};
