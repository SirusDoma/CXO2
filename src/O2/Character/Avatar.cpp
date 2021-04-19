#include <O2/Character/Avatar.hpp>

Avatar::Avatar() :
    m_playerInfo(),
    m_instrument(Equipment::Instrument::None),
    m_items(),
    m_defaultItems()
{
}

Avatar::Avatar(Room::PlayerInfo playerInfo) :
    m_playerInfo(),
    m_instrument(Equipment::Instrument::None),
    m_items(),
    m_defaultItems()
{
    SetPlayerInfo(playerInfo);
}

const Room::PlayerInfo &Avatar::GetPlayerInfo() const
{
    return m_playerInfo;
}

void Avatar::SetPlayerInfo(const Room::PlayerInfo &playerInfo)
{
    m_playerInfo = playerInfo;
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
        if (item->GetGender() != Character::Gender::Any && item->GetGender() != m_playerInfo.Gender)
            return;

        m_items[item->GetType()] = item;
        switch (item->GetType())
        {
            case Equipment::Type::Piano:  m_instrument = Equipment::Instrument::Piano;  break;
            case Equipment::Type::Bass:   m_instrument = Equipment::Instrument::Bass;   break;
            case Equipment::Type::Drum:   m_instrument = Equipment::Instrument::Drum;   break;
            case Equipment::Type::Guitar: m_instrument = Equipment::Instrument::Guitar; break;
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
            case Equipment::Type::Piano:
            case Equipment::Type::Bass:
            case Equipment::Type::Drum:
            case Equipment::Type::Guitar:
                m_instrument = Equipment::Instrument::None;
                break;
            default: break;
        }

        m_items.erase(iterator);

        iterator = m_defaultItems.find(item->GetType());
        if (iterator != m_items.end())
            Equip(iterator->second);
    }
}

const Equipment::Instrument &Avatar::GetEquipedInstrumentType() const
{
    return m_instrument;
}

const std::map<Equipment::Type, const Item *> &Avatar::GetEquipedItems() const
{
    return m_items;
}

void Avatar::Update(double delta)
{
    UpdatableContainer::Update(delta);

    for (auto [_, item] : m_items)
    {
        for (auto animation : item->GetRenderables())
            animation->Update(delta);
    }
}

sf::RenderStates Avatar::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= GetTransform();
    auto iterator = m_items.find(Equipment::Type::Costume);
    if (iterator != m_items.end())
    {
        auto animation = iterator->second->GetRenderableItem(m_playerInfo.Gender, Equipment::RenderPart::Body, Equipment::Instrument::None);
        if (animation)
        {
            animation->Render(target, states);
            return RenderableContainer::Render(target, states);
        }
    }

    for (auto part : RENDER_PART_ORDER)
    {
        for (auto type : RENDER_TYPE_ORDER)
        {
            iterator = m_items.find(type);
            if (iterator == m_items.end())
                continue;

            auto animation = iterator->second->GetRenderableItem(m_playerInfo.Gender, part, m_instrument);
            if (animation)
                animation->Render(target, states);
        }
    }

    return RenderableContainer::Render(target, states);
}
