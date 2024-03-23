#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>

Avatar::Avatar() :
    m_player(),
    m_instrument(Instrument::None),
    m_items(),
    m_defaultItems()
{
}

Avatar::Avatar(Player playerInfo) :
    m_player(),
    m_instrument(Instrument::None),
    m_items(),
    m_defaultItems()
{
    SetPlayer(playerInfo);
}

const Player &Avatar::GetPlayer() const
{
    return m_player;
}

void Avatar::SetPlayer(const Player &playerInfo)
{
    m_player = playerInfo;
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
        if (item->GetGender() != Gender::Any && item->GetGender() != m_player.Gender)
            return;

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
    UpdatableContainer::Update(delta);

    for (auto [type, part] : RENDER_ORDER)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            continue;

        auto animation = iterator->second->GetRenderableItem(m_player.Gender, part, m_instrument);
        if (animation)
            animation->Update(delta);

        if (type == EquipmentType::Costume && part == RenderPart::Body)
            break;
    }
}

sf::RenderStates Avatar::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= GetTransform();

    for (auto [type, part] : RENDER_ORDER)
    {
        auto iterator = m_items.find(type);
        if (iterator == m_items.end())
            continue;

        auto animation = iterator->second->GetRenderableItem(m_player.Gender, part, m_instrument);
        if (animation)
            animation->Render(target, states);

        if (type == EquipmentType::Costume && part == RenderPart::Body)
            break;
    }

    return RenderableContainer::Render(target, states);
}
