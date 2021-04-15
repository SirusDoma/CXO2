#include <O2/Character/Avatar.hpp>

Avatar::Avatar(Room::PlayerInfo playerInfo) :
    m_playerInfo(playerInfo),
    m_instrument(Equipment::Instrument::None),
    m_items()
{
}

const Room::PlayerInfo &Avatar::GetPlayerInfo() const
{
    return m_playerInfo;
}

void Avatar::Equip(Item *item)
{
    if (item)
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
        auto find = m_items.find(type);
        if (find == m_items.end())
            continue;

        auto item = find->second;
        for (auto renderType : PART_RENDER_ORDER)
        {
            auto animation = item->GetRenderableItem(m_playerInfo.Gender, renderType, m_instrument);
            if (animation)
                animation->Render(target, states);
        }
    }

    return RenderableContainer::Render(target, states);
}

