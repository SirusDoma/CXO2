#include <O2/Character/Item.hpp>


Item::Item() :
    m_prices(),
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

bool Item::IsBuyableWith(const Shop::Currency &currency) const
{
    return m_prices.find(currency) != m_prices.end();
}

unsigned int Item::GetPrice(const Shop::Currency &currency) const
{
    auto iterator = m_prices.find(currency);
    if (iterator != m_prices.end())
        return iterator->second;

    return 0;
}

void Item::SetPrice(const Shop::Currency &currency, unsigned int price)
{
    m_prices[currency] = price;
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

void Item::SetRenderableItem(Character::Gender gender, Equipment::RenderPart renderType, Equipment::Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation)
{
    if (animation)
        m_renderables[RenderableKey(gender, renderType, instrument)] = std::move(animation);
}

Gx::Animation *Item::GetRenderableItem(Character::Gender gender, Equipment::RenderPart renderType, Equipment::Instrument instrument) const
{
    auto find = m_renderables.find(RenderableKey(gender, renderType, instrument));
    if (find != m_renderables.end())
        return find->second.get();

    return nullptr;
}

std::vector<Gx::Animation *> Item::GetRenderables() const
{
    auto renderables = std::vector<Gx::Animation *>();
    for (auto& [_, renderable] : m_renderables)
        renderables.push_back(renderable.get());

    return renderables;
}


void Item::ResetRenderables()
{
    for (auto& [key, renderable] : m_renderables)
        renderable->Reset();
}
