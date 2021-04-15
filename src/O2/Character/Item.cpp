#include <O2/Character/Item.hpp>


Item::Item() :
    m_smallPreview(),
    m_largePreview(),
    m_renderables()
{
}

unsigned int Item::GetId() const
{
    return 0;
}

void Item::SetId(unsigned int id)
{
    m_id = id;
}

Equipment::Type Item::GetType() const
{
    return m_type;
}

void Item::SetType(Equipment::Type type)
{
    m_type = type;
}

Planet::Planet Item::GetOrigin() const
{
    return m_origin;
}

void Item::SetOrigin(Planet::Planet planet)
{
    m_origin = planet;
}

Character::Gender Item::GetGender() const
{
    return m_gender;
}

void Item::SetGender(Character::Gender gender)
{
    m_gender = gender;
}

bool Item::IsNew() const
{
    return m_isNew;
}

void Item::SetIsNew(bool isNew)
{
    m_isNew = isNew;
}

Shop::Currency Item::GetPriceCurrency() const
{
    return m_priceCurrency;
}

void Item::SetPriceCurrency(Shop::Currency currency)
{
    m_priceCurrency = currency;
}

unsigned int Item::GetPrice() const
{
    return m_price;
}

void Item::SetPrice(unsigned int price)
{
    m_price = price;
}

const sf::String &Item::GetName() const
{
    return m_name;
}

void Item::SetName(const sf::String &name)
{
    m_name = name;
}

const sf::String &Item::GetDescription() const
{
    return m_description;
}

void Item::SetDescription(const sf::String &description)
{
    m_description = description;
}

const Gx::Sprite *Item::GetSmallPreview() const
{
    return m_smallPreview.get();
}

void Item::SetSmallPreview(Gx::ResourcePtr<Gx::Sprite> smallPreview)
{
    if (smallPreview)
        m_smallPreview = std::move(smallPreview);
}

const Gx::Sprite *Item::GetLargePreview() const
{
    return m_largePreview.get();
}

void Item::SetLargePreview(Gx::ResourcePtr<Gx::Sprite> largePreview)
{
    if (largePreview)
        m_largePreview = std::move(largePreview);
}

void Item::SetRenderableItem(Character::Gender gender, Equipment::RenderType renderType, Equipment::Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation)
{
    if (animation)
        m_renderables[RenderableKey(gender, renderType, instrument)] = std::move(animation);
}

Gx::Animation *Item::GetRenderableItem(Character::Gender gender, Equipment::RenderType renderType, Equipment::Instrument instrument) const
{
    auto find = m_renderables.find(RenderableKey(gender, renderType, instrument));
    if (find != m_renderables.end())
        return find->second.get();

    return nullptr;
}

void Item::ResetRenderables()
{
    for (auto& [key, renderable] : m_renderables)
        renderable->Reset();
}
