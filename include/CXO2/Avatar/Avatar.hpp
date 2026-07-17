#pragma once

#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Models/Room.hpp>
#include <CXO2/UI/Waiting/AvatarInfo.hpp>

#include <Genode/SceneGraph.hpp>

#include <unordered_map>

namespace Cx
{
    class Avatar : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
    {
    public:
        using ItemMap = std::unordered_map<EquipmentType, Item>;

        Avatar() = default;
        explicit Avatar(Gender gender);

        Gender GetGender() const;
        void SetGender(Gender gender);

        void SetDefaultItem(const Item& item);

        bool IsEquiped(const Item& item) const;
        void Equip(const Item& item);
        void Unequip(const Item& item);
        void Unequip(EquipmentType type);
        void ClearEquipments();

        sf::Vector2f GetOffset() const;
        void SetOffset(const sf::Vector2f& offset);

        AvatarInfo* GetAvatarInfo() const;
        Instrument GetEquipedInstrumentType() const;
        std::unordered_map<EquipmentType, const Item*> GetEquipedItems(bool includeDefaultItems = false) const;

        bool IsAlive() const;
        void Die();
        void Revive();

        void ResetRenderables();

    private:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void UpdateOhmEffect();

        Item* FindItem(EquipmentType type);
        const Item* FindItem(EquipmentType type) const;

        Gender       m_gender{};
        Instrument   m_instrument{Instrument::None};
        bool         m_alive{true};
        sf::Vector2f m_offset;
        ItemMap      m_items, m_defaultItems;
    };
}
