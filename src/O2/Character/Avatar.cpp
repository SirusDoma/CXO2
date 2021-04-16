#include <O2/Character/Avatar.hpp>

Avatar::Avatar() :
    m_playerInfo(),
    m_instrument(Equipment::Instrument::None),
    m_items(),
    m_defaultItems()
{
}

Avatar::Avatar(Room::PlayerInfo playerInfo) :
    m_playerInfo(playerInfo),
    m_instrument(Equipment::Instrument::None),
    m_items(),
    m_defaultItems()
{
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
        m_items[item->GetType()] = item;
        switch (item->GetType())
        {
            case Equipment::Type::Piano:  m_instrument = Equipment::Instrument::Piano;  break;
            case Equipment::Type::Bass:   m_instrument = Equipment::Instrument::Bass;   break;
            case Equipment::Type::Drum:   m_instrument = Equipment::Instrument::Drum;   break;
            case Equipment::Type::Guitar: m_instrument = Equipment::Instrument::Guitar; break;
            default: break;
        }

        //for (auto [_, item] : m_items)
        //    item->ResetRenderables();
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
        for (auto renderType : PART_RENDER_ORDER)
        {
            for (auto instrument : INSTRUMENT_RENDER_ORDER)
            {
                auto animation = item->GetRenderableItem(m_playerInfo.Gender, renderType, instrument);
                if (animation)
                    animation->Update(delta);
            }
        }
    }
}

sf::RenderStates Avatar::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= GetTransform();
    for (auto type : TYPE_RENDER_ORDER)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            continue;

        auto item = iterator->second;
        for (auto renderType : PART_RENDER_ORDER)
        {
            auto animation = item->GetRenderableItem(m_playerInfo.Gender, renderType, m_instrument);
            if (animation)
                animation->Render(target, states);
        }
    }

    return RenderableContainer::Render(target, states);
}
