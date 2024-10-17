#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Models/Equipment.hpp>

static constexpr std::array<std::pair<EquipmentType, RenderPart>, 100> RenderLayerOrder =
{
    {
        { EquipmentType::Costume,               RenderPart::Cape },
        { EquipmentType::Costume,               RenderPart::Body },
        { EquipmentType::Costume,               RenderPart::LeftArm },
        { EquipmentType::Costume,               RenderPart::RightArm },
        { EquipmentType::Accessories,           RenderPart::Cape },
        { EquipmentType::Accessories,           RenderPart::Body },
        { EquipmentType::Accessories,           RenderPart::LeftArm },
        { EquipmentType::Accessories,           RenderPart::RightArm },
        { EquipmentType::Wings,                 RenderPart::Cape },
        { EquipmentType::Wings,                 RenderPart::Body },
        { EquipmentType::Wings,                 RenderPart::LeftArm },
        { EquipmentType::Wings,                 RenderPart::RightArm },
        { EquipmentType::Body,                  RenderPart::Cape },
        { EquipmentType::Top,                   RenderPart::Cape },
        { EquipmentType::Hair,                  RenderPart::Cape },
        { EquipmentType::Body,                  RenderPart::Body },
        { EquipmentType::Body,                  RenderPart::LeftArm },
        { EquipmentType::Body,                  RenderPart::RightArm },
        { EquipmentType::LeftArm,               RenderPart::Cape },
        { EquipmentType::LeftArm,               RenderPart::LeftArm },
        { EquipmentType::LeftArm,               RenderPart::RightArm },
        { EquipmentType::RightArm,              RenderPart::Cape },
        { EquipmentType::RightArm,              RenderPart::LeftArm },
        { EquipmentType::RightArm,              RenderPart::RightArm },
        { EquipmentType::Shoes,                 RenderPart::Cape },
        { EquipmentType::Shoes,                 RenderPart::Body },
        { EquipmentType::Shoes,                 RenderPart::LeftArm },
        { EquipmentType::Shoes,                 RenderPart::RightArm },
        { EquipmentType::Pants,                 RenderPart::Cape },
        { EquipmentType::Pants,                 RenderPart::Body },
        { EquipmentType::Pants,                 RenderPart::LeftArm },
        { EquipmentType::Pants,                 RenderPart::RightArm },
        { EquipmentType::ClothesAccessories,    RenderPart::Cape },
        { EquipmentType::ClothesAccessories,    RenderPart::Body },
        { EquipmentType::ClothesAccessories,    RenderPart::LeftArm },
        { EquipmentType::ClothesAccessories,    RenderPart::RightArm },
        { EquipmentType::Keyboard,              RenderPart::Cape },
        { EquipmentType::Keyboard,              RenderPart::Body },
        { EquipmentType::Keyboard,              RenderPart::LeftArm },
        { EquipmentType::Keyboard,              RenderPart::RightArm },
        { EquipmentType::LeftArm,               RenderPart::Body },
        { EquipmentType::Top,                   RenderPart::Body },
        { EquipmentType::Top,                   RenderPart::LeftArm },
        { EquipmentType::Bass,                  RenderPart::Cape },
        { EquipmentType::Bass,                  RenderPart::Body },
        { EquipmentType::Bass,                  RenderPart::LeftArm },
        { EquipmentType::Bass,                  RenderPart::RightArm },
        { EquipmentType::Guitar,                RenderPart::Cape },
        { EquipmentType::Guitar,                RenderPart::Body },
        { EquipmentType::Guitar,                RenderPart::LeftArm },
        { EquipmentType::Guitar,                RenderPart::RightArm },
        { EquipmentType::RightArm,              RenderPart::Body },
        { EquipmentType::Top,                   RenderPart::RightArm },
        { EquipmentType::Face,                  RenderPart::Cape },
        { EquipmentType::Face,                  RenderPart::Body },
        { EquipmentType::Face,                  RenderPart::LeftArm },
        { EquipmentType::Face,                  RenderPart::RightArm },
        { EquipmentType::Earrings,              RenderPart::Cape },
        { EquipmentType::Earrings,              RenderPart::Body },
        { EquipmentType::Earrings,              RenderPart::LeftArm },
        { EquipmentType::Earrings,              RenderPart::RightArm },
        { EquipmentType::Necklace,              RenderPart::Cape },
        { EquipmentType::Necklace,              RenderPart::Body },
        { EquipmentType::Necklace,              RenderPart::LeftArm },
        { EquipmentType::Necklace,              RenderPart::RightArm },
        { EquipmentType::Glasses,               RenderPart::Cape },
        { EquipmentType::Glasses,               RenderPart::Body },
        { EquipmentType::Glasses,               RenderPart::LeftArm },
        { EquipmentType::Glasses,               RenderPart::RightArm },
        { EquipmentType::Hair,                  RenderPart::Body },
        { EquipmentType::Hair,                  RenderPart::LeftArm },
        { EquipmentType::Hair,                  RenderPart::RightArm },
        { EquipmentType::HairAccessories,       RenderPart::Cape },
        { EquipmentType::HairAccessories,       RenderPart::Body },
        { EquipmentType::HairAccessories,       RenderPart::LeftArm },
        { EquipmentType::HairAccessories,       RenderPart::RightArm },
        { EquipmentType::Pet,                   RenderPart::Cape },
        { EquipmentType::Pet,                   RenderPart::Body },
        { EquipmentType::Pet,                   RenderPart::LeftArm },
        { EquipmentType::Pet,                   RenderPart::RightArm },
        { EquipmentType::LeftHand,              RenderPart::Cape },
        { EquipmentType::LeftHand,              RenderPart::Body },
        { EquipmentType::LeftHand,              RenderPart::LeftArm },
        { EquipmentType::LeftHand,              RenderPart::RightArm },
        { EquipmentType::RightHand,             RenderPart::Cape },
        { EquipmentType::RightHand,             RenderPart::Body },
        { EquipmentType::RightHand,             RenderPart::LeftArm },
        { EquipmentType::RightHand,             RenderPart::RightArm },
        { EquipmentType::Gloves,                RenderPart::Cape },
        { EquipmentType::Gloves,                RenderPart::Body },
        { EquipmentType::Gloves,                RenderPart::LeftArm },
        { EquipmentType::Gloves,                RenderPart::RightArm },
        { EquipmentType::Drum,                  RenderPart::Cape },
        { EquipmentType::Drum,                  RenderPart::Body },
        { EquipmentType::Drum,                  RenderPart::LeftArm },
        { EquipmentType::Drum,                  RenderPart::RightArm },
        { EquipmentType::InstrumentAccessories, RenderPart::Cape },
        { EquipmentType::InstrumentAccessories, RenderPart::Body },
        { EquipmentType::InstrumentAccessories, RenderPart::LeftArm },
        { EquipmentType::InstrumentAccessories, RenderPart::RightArm },
    }
};

Avatar::Avatar() :
    m_gender(),
    m_instrument(Instrument::None),
    m_alive(true),
    m_items(),
    m_defaultItems()
{
}

Avatar::Avatar(const Gender gender) :
    Avatar()
{
    m_gender = gender;
}

void Avatar::Initialize()
{
    Gx::Node::Initialize();
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
    if (item.GetID() == 0)
        return;

    if (IsEquiped(item))
        return;

    // Equip only equippable item
    if (!item.IsEquipable())
        return;

    // Check whether the item gender is matching
    if (item.GetGender() != Gender::Any && item.GetGender() != m_gender)
        return;

    // Check whether the item is already equipped
    if (const auto equipped = m_items.find(item.GetType()); equipped != m_items.end() && equipped->second.GetID() == item.GetID())
        return;

    // Check whether a costume is currently equipped
    if (const auto equipped = m_items.find(EquipmentType::Costume); equipped != m_items.end())
        return;

    // Unequip existing instrument if the item is an instrument
    if (item.GetInstrument() != Instrument::None)
    {
        for (const auto type : { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Drum, EquipmentType::Keyboard })
            Unequip(type);
    }

    // Remove all equipments if the item is costume
    if (item.GetType() == EquipmentType::Costume)
        ClearEquipments();

    m_items[item.GetType()] = std::move(item);
    switch (item.GetType())
    {
        case EquipmentType::Keyboard: m_instrument = Instrument::Keyboard;  break;
        case EquipmentType::Bass:     m_instrument = Instrument::Bass;   break;
        case EquipmentType::Drum:     m_instrument = Instrument::Drum;   break;
        case EquipmentType::Guitar:   m_instrument = Instrument::Guitar; break;
        default: break;
    }

    ResetRenderables();
}

void Avatar::Unequip(const Item& item)
{
    if (item.GetID() == 0)
        return;

    const auto iterator = m_items.find(item.GetType());
    if (iterator == m_items.end())
        return;

    if (iterator->second.GetID() != item.GetID())
        return;

    switch (item.GetType())
    {
        case EquipmentType::Keyboard:
        case EquipmentType::Bass:
        case EquipmentType::Drum:
        case EquipmentType::Guitar:
            m_instrument = Instrument::None;
        break;
        default: break;
    }

    m_items.erase(iterator);
    ResetRenderables();
}

void Avatar::Unequip(const EquipmentType type)
{
    const auto iterator = m_items.find(type);
    if (iterator == m_items.end())
        return;

    switch (type)
    {
        case EquipmentType::Keyboard:
        case EquipmentType::Bass:
        case EquipmentType::Drum:
        case EquipmentType::Guitar:
            m_instrument = Instrument::None;
            break;
        default: break;
    }

    m_items.erase(iterator);
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

void Avatar::ResetRenderables() const
{
    for (auto& [_, item] : m_items)
    {
        for (const auto renderable : item.GetRenderables())
            renderable->Reset();
    }

    for (auto& [_, item] : m_defaultItems)
    {
        for (const auto renderable : item.GetRenderables())
            renderable->Reset();
    }
}

AvatarInfo* Avatar::GetAvatarInfo() const
{
    return FindChild<AvatarInfo>("IDC_AVATAR_INFO");
}

const Instrument& Avatar::GetEquipedInstrumentType() const
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

void Avatar::Update(const double delta)
{
    const auto ohmEffect = FindChild<Gx::Animation>("IDC_ANIMATION_OHM_EFFECT");
    const auto ohm = FindChild<Gx::Animation>("IDC_ANIMATION_OHM");

    if (m_alive)
    {
        if (ohmEffect)
        {
            ohmEffect->Stop();
            ohmEffect->SetVisible(false);
        }

        if (ohm)
            ohm->SetVisible(false);
    }
    else if (ohmEffect && ohm && ohmEffect->GetState() != Gx::Animation::AnimationState::Playing && ohmEffect->GetState() != Gx::Animation::AnimationState::Completed)
    {
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

    for (auto [type, part] : RenderLayerOrder)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
        {
            iterator = m_defaultItems.find(type);
            if (iterator == m_defaultItems.end())
                continue;
        }

        if (const auto animation = iterator->second.GetRenderableItem(m_gender, part, m_instrument))
            animation->Update(delta);

        if (type == EquipmentType::Costume && part == RenderPart::Body)
            break;
    }

    UpdatableContainer::Update(delta);
}

Gx::RenderStates Avatar::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    states.transform *= GetTransform();
    if (!m_alive)
        return RenderableContainer::Render(surface, states);

    for (auto [type, part] : RenderLayerOrder)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
        {
            iterator = m_defaultItems.find(type);
            if (iterator == m_defaultItems.end())
                continue;
        }

        if (const auto& animation = iterator->second.GetRenderableItem(m_gender, part, m_instrument))
            animation->Render(surface, states);

        states.Layer += 1.f;
        if (type == EquipmentType::Costume && part == RenderPart::Body)
            break;
    }

    return RenderableContainer::Render(surface, states);
}

void Avatar::ClearEquipments()
{
    m_items.clear();
    m_instrument = Instrument::None;

    ResetRenderables();
}
