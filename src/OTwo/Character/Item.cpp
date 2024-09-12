#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Shop.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Equipment.hpp>

unsigned int Item::GetID() const
{
    return m_id;
}

void Item::SetID(const unsigned int id)
{
    m_id = id;
}

EquipmentType Item::GetType() const
{
    return m_type;
}

void Item::SetType(const EquipmentType type)
{
    m_type = type;
}

bool Item::IsEquipable() const
{
    return m_type != EquipmentType::AttributiveItem;
}

Planet Item::GetOrigin() const
{
    return m_origin;
}

void Item::SetOrigin(const Planet planet)
{
    m_origin = planet;
}

Gender Item::GetGender() const
{
    return m_gender;
}

void Item::SetGender(const Gender gender)
{
    m_gender = gender;
}

bool Item::IsNew() const
{
    return m_isNew;
}

void Item::SetIsNew(const bool isNew)
{
    m_isNew = isNew;
}

const sf::String& Item::GetName() const
{
    return m_name;
}

void Item::SetName(const sf::String& name)
{
    m_name = name;
}

const sf::String& Item::GetDescription() const
{
    return m_description;
}

void Item::SetDescription(const sf::String& description)
{
    m_description = description;
}

bool Item::IsBuyableWith(const Currency& currency) const
{
    return m_prices.find(currency) != m_prices.end();
}

unsigned int Item::GetPrice(const Currency& currency) const
{
    const auto iterator = m_prices.find(currency);
    if (iterator != m_prices.end())
        return iterator->second;

    return 0;
}

void Item::SetPrice(const Currency& currency, const unsigned int price)
{
    m_prices[currency] = price;
}

const Gx::Sprite* Item::GetSmallPreview() const
{
    return m_smallPreview.get();
}

void Item::SetSmallPreview(Gx::ResourcePtr<Gx::Sprite> smallPreview)
{
    if (smallPreview)
        m_smallPreview = std::move(smallPreview);
}

const Gx::Sprite* Item::GetLargePreview() const
{
    return m_largePreview.get();
}

void Item::SetLargePreview(Gx::ResourcePtr<Gx::Sprite> largePreview)
{
    if (largePreview)
        m_largePreview = std::move(largePreview);
}

void Item::SetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation)
{
    if (animation)
        m_renderables[RenderableKey(gender, renderType, instrument)] = std::move(animation);
}

Gx::Animation* Item::GetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument) const
{
    const auto find = m_renderables.find(RenderableKey(gender, renderType, instrument));
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

Instrument Item::GetInstrument() const
{
    switch(m_type)
    {
        case EquipmentType::Bass:     return Instrument::Bass;
        case EquipmentType::Guitar:   return Instrument::Guitar;
        case EquipmentType::Keyboard: return Instrument::Keyboard;
        case EquipmentType::Drum:     return Instrument::Drum;
        default:                      return Instrument::None;
    }
}
