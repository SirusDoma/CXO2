#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Models/Equipment.hpp>

#include <CXO2/StringTable/Identifiers/Avatar.hpp>

#include <utility>

namespace Cx
{
    using namespace StringTable::Identifiers;

    static constexpr std::array<std::pair<EquipmentType, RenderPart>, 100> RenderLayerOrder =
    {
        {
            { EquipmentType::Costume,               RenderPart::Back },
            { EquipmentType::Costume,               RenderPart::Body },
            { EquipmentType::Costume,               RenderPart::LeftArm },
            { EquipmentType::Costume,               RenderPart::RightArm },
            { EquipmentType::Accessories,           RenderPart::Back },
            { EquipmentType::Accessories,           RenderPart::Body },
            { EquipmentType::Accessories,           RenderPart::LeftArm },
            { EquipmentType::Accessories,           RenderPart::RightArm },
            { EquipmentType::Wings,                 RenderPart::Back },
            { EquipmentType::Wings,                 RenderPart::Body },
            { EquipmentType::Wings,                 RenderPart::LeftArm },
            { EquipmentType::Wings,                 RenderPart::RightArm },
            { EquipmentType::Body,                  RenderPart::Back },
            { EquipmentType::Hair,                  RenderPart::Back },
            { EquipmentType::Top,                   RenderPart::Back },
            { EquipmentType::Body,                  RenderPart::Body },
            { EquipmentType::Body,                  RenderPart::LeftArm },
            { EquipmentType::Body,                  RenderPart::RightArm },
            { EquipmentType::Shoes,                 RenderPart::Back },
            { EquipmentType::Shoes,                 RenderPart::Body },
            { EquipmentType::Shoes,                 RenderPart::LeftArm },
            { EquipmentType::Shoes,                 RenderPart::RightArm },
            { EquipmentType::Pants,                 RenderPart::Back },
            { EquipmentType::Pants,                 RenderPart::Body },
            { EquipmentType::Pants,                 RenderPart::LeftArm },
            { EquipmentType::Pants,                 RenderPart::RightArm },
            { EquipmentType::ClothesAccessories,    RenderPart::Back },
            { EquipmentType::ClothesAccessories,    RenderPart::Body },
            { EquipmentType::ClothesAccessories,    RenderPart::LeftArm },
            { EquipmentType::ClothesAccessories,    RenderPart::RightArm },
            { EquipmentType::LeftArm,               RenderPart::Back },
            { EquipmentType::LeftArm,               RenderPart::Body },
            { EquipmentType::LeftArm,               RenderPart::LeftArm },
            { EquipmentType::LeftArm,               RenderPart::RightArm },
            { EquipmentType::Keyboard,              RenderPart::Back },
            { EquipmentType::Keyboard,              RenderPart::Body },
            { EquipmentType::Keyboard,              RenderPart::LeftArm },
            { EquipmentType::Keyboard,              RenderPart::RightArm },
            { EquipmentType::Top,                   RenderPart::Body },
            { EquipmentType::Top,                   RenderPart::LeftArm },
            { EquipmentType::Bass,                  RenderPart::Back },
            { EquipmentType::Bass,                  RenderPart::Body },
            { EquipmentType::Bass,                  RenderPart::LeftArm },
            { EquipmentType::Bass,                  RenderPart::RightArm },
            { EquipmentType::Guitar,                RenderPart::Back },
            { EquipmentType::Guitar,                RenderPart::Body },
            { EquipmentType::Guitar,                RenderPart::LeftArm },
            { EquipmentType::Guitar,                RenderPart::RightArm },
            { EquipmentType::RightArm,              RenderPart::Back },
            { EquipmentType::RightArm,              RenderPart::Body },
            { EquipmentType::RightArm,              RenderPart::LeftArm },
            { EquipmentType::RightArm,              RenderPart::RightArm },
            { EquipmentType::Top,                   RenderPart::RightArm },
            { EquipmentType::Necklace,              RenderPart::Back },
            { EquipmentType::Necklace,              RenderPart::Body },
            { EquipmentType::Necklace,              RenderPart::LeftArm },
            { EquipmentType::Necklace,              RenderPart::RightArm },
            { EquipmentType::LeftHand,              RenderPart::Back },
            { EquipmentType::LeftHand,              RenderPart::Body },
            { EquipmentType::LeftHand,              RenderPart::LeftArm },
            { EquipmentType::LeftHand,              RenderPart::RightArm },
            { EquipmentType::RightHand,             RenderPart::Back },
            { EquipmentType::RightHand,             RenderPart::Body },
            { EquipmentType::RightHand,             RenderPart::LeftArm },
            { EquipmentType::RightHand,             RenderPart::RightArm },
            { EquipmentType::Gloves,                RenderPart::Back },
            { EquipmentType::Gloves,                RenderPart::Body },
            { EquipmentType::Gloves,                RenderPart::LeftArm },
            { EquipmentType::Gloves,                RenderPart::RightArm },
            { EquipmentType::Face,                  RenderPart::Back },
            { EquipmentType::Face,                  RenderPart::Body },
            { EquipmentType::Face,                  RenderPart::LeftArm },
            { EquipmentType::Face,                  RenderPart::RightArm },
            { EquipmentType::Earrings,              RenderPart::Back },
            { EquipmentType::Earrings,              RenderPart::Body },
            { EquipmentType::Earrings,              RenderPart::LeftArm },
            { EquipmentType::Earrings,              RenderPart::RightArm },
            { EquipmentType::Glasses,               RenderPart::Back },
            { EquipmentType::Glasses,               RenderPart::Body },
            { EquipmentType::Glasses,               RenderPart::LeftArm },
            { EquipmentType::Glasses,               RenderPart::RightArm },
            { EquipmentType::Hair,                  RenderPart::Body },
            { EquipmentType::Hair,                  RenderPart::LeftArm },
            { EquipmentType::Hair,                  RenderPart::RightArm },
            { EquipmentType::HairAccessories,       RenderPart::Back },
            { EquipmentType::HairAccessories,       RenderPart::Body },
            { EquipmentType::HairAccessories,       RenderPart::LeftArm },
            { EquipmentType::HairAccessories,       RenderPart::RightArm },
            { EquipmentType::Drum,                  RenderPart::Back },
            { EquipmentType::Drum,                  RenderPart::Body },
            { EquipmentType::Drum,                  RenderPart::LeftArm },
            { EquipmentType::Drum,                  RenderPart::RightArm },
            { EquipmentType::InstrumentAccessories, RenderPart::Back },
            { EquipmentType::InstrumentAccessories, RenderPart::Body },
            { EquipmentType::InstrumentAccessories, RenderPart::LeftArm },
            { EquipmentType::InstrumentAccessories, RenderPart::RightArm },
            { EquipmentType::Pet,                   RenderPart::Back },
            { EquipmentType::Pet,                   RenderPart::Body },
            { EquipmentType::Pet,                   RenderPart::LeftArm },
            { EquipmentType::Pet,                   RenderPart::RightArm },
        }
    };

    Avatar::Avatar(const Gender gender) :
        m_gender(gender)
    {
    }

    Gender Avatar::GetGender() const
    {
        return m_gender;
    }

    void Avatar::SetGender(const Gender gender)
    {
        m_gender = gender;
    }

    void Avatar::SetDefaultItem(const Item& item)
    {
        ClearEquipments();

        if (item.GetID() != 0)
            m_defaultItems[item.GetType()] = item;

        ResetRenderables();
    }

    bool Avatar::IsEquiped(const Item& item) const
    {
        if (item.GetID() == 0)
            return false;

        const auto iterator = m_items.find(item.GetType());
        return iterator != m_items.end() && iterator->second.GetID() == item.GetID();
    }

    void Avatar::Equip(const Item& item)
    {
        if (item.GetID() == 0 || IsEquiped(item) || !item.IsEquipable())
            return;

        // Check whether the item gender is matching
        if (item.GetGender() != Gender::Any && item.GetGender() != m_gender)
            return;

        if (m_items.find(EquipmentType::Costume) != m_items.end())
            return;

        const auto instrument = item.GetInstrument();
        if (instrument != Instrument::None)
        {
            for (const auto type : { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Drum, EquipmentType::Keyboard })
                Unequip(type);
        }

        // Remove all equipments if the item is costume
        if (item.GetType() == EquipmentType::Costume)
            ClearEquipments();

        m_items[item.GetType()] = item;
        if (instrument != Instrument::None)
            m_instrument = instrument;

        ResetRenderables();
    }

    void Avatar::Unequip(const Item& item)
    {
        if (item.GetID() == 0)
            return;

        const auto iterator = m_items.find(item.GetType());
        if (iterator == m_items.end() || iterator->second.GetID() != item.GetID())
            return;

        Unequip(item.GetType());
    }

    void Avatar::Unequip(const EquipmentType type)
    {
        const auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            return;

        if (iterator->second.GetInstrument() != Instrument::None)
            m_instrument = Instrument::None;

        m_items.erase(iterator);
        ResetRenderables();
    }

    void Avatar::ClearEquipments()
    {
        m_items.clear();
        m_instrument = Instrument::None;

        ResetRenderables();
    }

    bool Avatar::IsAlive() const
    {
        return m_alive;
    }

    void Avatar::Die()
    {
        m_alive = false;
    }

    void Avatar::Revive()
    {
        m_alive = true;
    }

    void Avatar::ResetRenderables()
    {
        for (auto& [_, item] : m_items)
            item.ResetRenderables();

        for (auto& [_, item] : m_defaultItems)
            item.ResetRenderables();
    }

    AvatarInfo* Avatar::GetAvatarInfo() const
    {
        return FindChild<AvatarInfo>(Resource::Avatar::IDC_AVATAR_INFO);
    }

    Instrument Avatar::GetEquipedInstrumentType() const
    {
        return m_instrument;
    }

    std::unordered_map<EquipmentType, const Item*> Avatar::GetEquipedItems(const bool includeDefaultItems) const
    {
        auto itemMap = std::unordered_map<EquipmentType, const Item*>();
        for (const auto& [type, item] : m_items)
        {
            if (auto it = m_defaultItems.find(type); it == m_defaultItems.end() || it->second.GetID() != item.GetID())
                itemMap[type] = &item;
        }

        if (!includeDefaultItems)
            return itemMap;

        for (const auto& [type, item] : m_defaultItems)
        {
            if (auto it = itemMap.find(type); it == itemMap.end())
                itemMap[type] = &item;
        }

        return itemMap;
    }

    sf::Vector2f Avatar::GetOffset() const
    {
        return m_offset;
    }

    void Avatar::SetOffset(const sf::Vector2f& offset)
    {
        m_offset = offset;
    }

    Item* Avatar::FindItem(const EquipmentType type)
    {
        return const_cast<Item*>(std::as_const(*this).FindItem(type));
    }

    const Item* Avatar::FindItem(const EquipmentType type) const
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
        {
            iterator = m_defaultItems.find(type);
            if (iterator == m_defaultItems.end())
                return nullptr;
        }

        return &iterator->second;
    }

    void Avatar::UpdateOhmEffect()
    {
        const auto ohmEffect = FindChild<Gx::Animation>(Resource::Avatar::IDC_ANIMATION_OHM_EFFECT);
        const auto ohm       = FindChild<Gx::Animation>(Resource::Avatar::IDC_ANIMATION_OHM);

        if (m_alive)
        {
            if (ohmEffect)
            {
                ohmEffect->Stop();
                ohmEffect->SetVisible(false);
            }

            if (ohm)
                ohm->SetVisible(false);

            return;
        }

        if (!ohmEffect || !ohm)
            return;

        if (ohmEffect->GetState() == Gx::Animation::AnimationState::Playing || ohmEffect->GetState() == Gx::Animation::AnimationState::Completed)
            return;

        ohmEffect->SetAnimationCallback([ohm] (auto& animation)
        {
            if (animation.GetState() == Gx::Animation::AnimationState::Completed)
            {
                animation.SetVisible(false);
                if (ohm)
                    ohm->SetVisible(true);
            }
        });

        ohmEffect->SetVisible(true);
        ohmEffect->Reset();
    }

    void Avatar::Update(const sf::Time& delta)
    {
        UpdateOhmEffect();

        for (const auto [type, part] : RenderLayerOrder)
        {
            if (const auto item = FindItem(type))
            {
                if (const auto animation = item->GetRenderableItem(m_gender, part, m_instrument))
                    animation->Update(delta);
            }
        }

        UpdatableContainer::Update(delta);
    }

    Gx::RenderStates Avatar::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();
        if (!m_alive)
            return RenderableContainer::Render(surface, states);

        auto offset = states;
        offset.transform.translate(m_offset);

        for (auto [type, part] : RenderLayerOrder)
        {
            // Special layer handling for keyboard
            if (m_instrument == Instrument::Keyboard && part == RenderPart::Body)
            {
                if (type == EquipmentType::LeftArm)
                    type = EquipmentType::Top;
                else if (type == EquipmentType::Top)
                    type = EquipmentType::LeftArm;
            }

            const auto item = FindItem(type);
            if (!item)
                continue;

            if (const auto animation = item->GetRenderableItem(m_gender, part, m_instrument))
                animation->Render(surface, offset);

            offset.Layer += 1.f;
            if (type == EquipmentType::Costume && part == RenderPart::Body)
                break;
        }

        states.Layer = offset.Layer;
        return RenderableContainer::Render(surface, states);
    }
}
