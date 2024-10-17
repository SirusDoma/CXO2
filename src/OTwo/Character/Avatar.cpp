#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Models/Equipment.hpp>

Avatar::Avatar() :
    m_gender(),
    m_instrument(Instrument::None),
    m_items(),
    m_defaultItems(),
    m_alive(true)
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
