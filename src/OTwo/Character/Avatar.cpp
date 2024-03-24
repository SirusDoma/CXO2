#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>

Avatar::RenderableStateMap Avatar::m_renderableStates;
unsigned int Avatar::m_lastFrameID = 0;

Avatar::Avatar() :
    m_gender(),
    m_instrument(Instrument::None),
    m_items(),
    m_defaultItems()
{
    m_renderableStates.clear();
}

Avatar::Avatar(Gender gender) :
    Avatar()
{
    m_gender = gender;
}

Gender Avatar::GetGender() const
{
    return m_gender;
}

void Avatar::SetGender(Gender gender)
{
    m_gender = gender;
}

void Avatar::SetDefaultItem(const Item *item)
{
    if (item)
    {
        m_defaultItems[item->GetType()] = item;
        if (auto it = m_items.find(item->GetType()); it == m_items.end())
            Equip(item);
    }
}

bool Avatar::IsEquiped(const Item *item) const
{
    return m_items.find(item->GetType()) == m_items.end();
}

void Avatar::Equip(const Item *item)
{
    if (item)
    {
        if (item->GetGender() != Gender::Any && item->GetGender() != m_gender)
            return;

        for (auto renderable : item->GetRenderables())
            renderable->Reset();

        m_items[item->GetType()] = item;
        switch (item->GetType())
        {
            case EquipmentType::Piano: m_instrument = Instrument::Piano;  break;
            case EquipmentType::Bass: m_instrument = Instrument::Bass;   break;
            case EquipmentType::Drum: m_instrument = Instrument::Drum;   break;
            case EquipmentType::Guitar: m_instrument = Instrument::Guitar; break;
            default: break;
        }
    }
}

void Avatar::Unequip(const Item *item)
{
    if (!item)
        return;

    auto iterator = m_items.find(item->GetType());
    if (iterator == m_items.end())
        return;

    if (iterator->second == item)
    {
        switch (item->GetType())
        {
            case EquipmentType::Piano:
            case EquipmentType::Bass:
            case EquipmentType::Drum:
            case EquipmentType::Guitar:
                m_instrument = Instrument::None;
                break;
            default: break;
        }

        m_items.erase(iterator);

        iterator = m_defaultItems.find(item->GetType());
        if (iterator != m_items.end())
            Equip(iterator->second);
    }
}

const Instrument &Avatar::GetEquipedInstrumentType() const
{
    return m_instrument;
}

const std::unordered_map<EquipmentType, const Item *> &Avatar::GetEquipedItems() const
{
    return m_items;
}

void Avatar::Update(double delta)
{
    // Item instance are shared between multiple instances of Avatar
    // Therefore, we need to avoid making multiple Update calls on the item animations.
    // To do that, we need "Frame ID" from the RenderStates. Thus, the animation update need to be done in Render()
    UpdatableContainer::Update(delta);
}

Gx::RenderStates Avatar::Render(sf::RenderTarget &target, Gx::RenderStates states) const
{
    states.transform *= GetTransform();

    // This will prevent static state map from growing non-stop
    if (m_lastFrameID != states.FrameID)
    {
        //m_renderableStates.clear();
        m_lastFrameID = states.FrameID;
    }

    for (auto [type, part] : RENDER_LAYER_ORDER)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            continue;

        auto animation = iterator->second->GetRenderableItem(m_gender, part, m_instrument);
        if (animation)
        {
            // Item and its Animation instances are shared across multiple instances of Avatar.
            // Make sure to update the animation only once, otherwise the animation might be played at speed-up pace
            if (m_renderableStates[animation] != states.FrameID)
                animation->Update(states.Delta);

            animation->Render(target, states);
            m_renderableStates[animation] = states.FrameID;
        }

        if (type == EquipmentType::Costume && part == RenderPart::Body)
            break;
    }

    return RenderableContainer::Render(target, states);
}

void Avatar::ClearEquipments()
{
    m_items.clear();
    m_instrument = Instrument::None;
}
