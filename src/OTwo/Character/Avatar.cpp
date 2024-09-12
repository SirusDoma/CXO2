#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Equipment.hpp>

Avatar::RenderableStateMap Avatar::m_renderableStates;
unsigned int Avatar::m_lastFrameID = 0;

Avatar::Avatar() :
    m_gender(),
    m_instrument(Instrument::None),
    m_items(),
    m_defaultItems(),
    m_alive(true),
    m_elapsed()
{
    m_renderableStates.clear();
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

void Avatar::SetDefaultItem(const Item* item)
{
    if (item)
    {
        m_defaultItems[item->GetType()] = item;
        if (const auto it = m_items.find(item->GetType()); it == m_items.end())
            Equip(item);
    }
}

bool Avatar::IsEquiped(const Item* item) const
{
    const auto iterator = m_items.find(item->GetType());
    return iterator != m_items.end() && iterator->second->GetID() == item->GetID();
}

void Avatar::Equip(const Item* item, const bool reset)
{
    if (item)
    {
        if (!item->IsEquipable())
            return;

        if (const auto equipped = m_items.find(item->GetType()); equipped != m_items.end() && equipped->second->GetID() == item->GetID())
            return;

        if (const auto equipped = m_items.find(EquipmentType::Costume); equipped != m_items.end())
            return;

        if (item->GetGender() != Gender::Any && item->GetGender() != m_gender)
            return;

        if (item->GetInstrument() != Instrument::None)
        {
            for (const auto type : { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Drum, EquipmentType::Keyboard })
                Unequip(type);
        }

        if (item->GetType() == EquipmentType::Costume)
            ClearEquipments();

        m_items[item->GetType()] = item;
        switch (item->GetType())
        {
            case EquipmentType::Keyboard: m_instrument = Instrument::Keyboard;  break;
            case EquipmentType::Bass:     m_instrument = Instrument::Bass;   break;
            case EquipmentType::Drum:     m_instrument = Instrument::Drum;   break;
            case EquipmentType::Guitar:   m_instrument = Instrument::Guitar; break;
            default: break;
        }

        if (reset)
        {
            for (const auto renderable: item->GetRenderables())
            {
                renderable->Reset();
                for (auto type : { EquipmentType::Body, EquipmentType::Top, EquipmentType::LeftArm, EquipmentType::RightArm, EquipmentType::LeftHand, EquipmentType::RightHand })
                {
                    if (auto itemRef = m_items.find(type); itemRef != m_items.end())
                    {
                        if (type == EquipmentType::Body)
                        {
                            for (const auto part: { RenderPart::LeftArm, RenderPart::RightArm })
                            {
                                if (const auto refRenderable = itemRef->second->GetRenderableItem(GetGender(), part, item->GetInstrument()); refRenderable)
                                    refRenderable->Reset();
                            }
                        }
                        else
                        {
                            for (const auto refRenderable : itemRef->second->GetRenderables())
                                refRenderable->Reset();
                        }
                    }
                }
            }
        }
    }
}

void Avatar::Unequip(const Item* item)
{
    if (!item)
        return;

    const auto iterator = m_items.find(item->GetType());
    if (iterator == m_items.end())
        return;

    if (iterator->second == item)
    {
        switch (item->GetType())
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
        if (const auto defaultIterator = m_defaultItems.find(item->GetType()); defaultIterator != m_defaultItems.end())
            Equip(defaultIterator->second);
    }
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
    if (const auto defaultIterator = m_defaultItems.find(type); defaultIterator != m_defaultItems.end())
        Equip(defaultIterator->second);
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
    m_renderableStates.clear();
    for (auto [_, item] : m_items)
    {
        for (const auto renderable : item->GetRenderables())
            renderable->Reset();
    }

    for (auto [_, item] : m_defaultItems)
    {
        for (const auto renderable : item->GetRenderables())
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
    if (includeDefaultItems)
        return m_items;

    auto result = std::unordered_map<EquipmentType, const Item*>();
    for (auto [type, item] : m_items)
    {
        if (auto it = m_defaultItems.find(type); it == m_defaultItems.end() || it->second != item)
            result[type] = item;
    }

    return result;
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

    // Item instance are shared between multiple instances of Avatar
    // Therefore, we need to avoid making multiple Update calls on the item animations.
    // To do that, we need "Frame ID" from the RenderStates. Thus, the animation update need to be done in Render()
    UpdatableContainer::Update(delta);
}

Gx::RenderStates Avatar::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    states.transform *= GetTransform();
    if (!m_alive)
        return RenderableContainer::Render(surface, states);

    // This will prevent static state map from growing non-stop
    if (m_lastFrameID != states.FrameID)
    {
        //m_renderableStates.clear();
        m_lastFrameID = states.FrameID;
    }

    m_elapsed += states.Delta;
    for (auto [type, part] : RenderLayerOrder)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            continue;

        if (const auto animation = iterator->second->GetRenderableItem(m_gender, part, m_instrument))
        {
            // Item and its Animation instances are shared across multiple instances of Avatar.
            // Make sure to update the animation only once, otherwise the animation might be played at speed-up pace
            if (m_renderableStates[animation] != states.FrameID)
                animation->Update(states.Delta);

            animation->Render(surface, states);
            m_renderableStates[animation] = states.FrameID;
        }

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

    for (auto [_, item] : m_defaultItems)
        Equip(item);
}
