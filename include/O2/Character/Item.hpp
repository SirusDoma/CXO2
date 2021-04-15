#ifndef O2JAM_CHARACTER_ITEM_HPP
#define O2JAM_CHARACTER_ITEM_HPP

#include <O2/Data/Character.hpp>
#include <O2/Data/Equipment.hpp>
#include <O2/Data/Planet.hpp>
#include <O2/Data/Shop.hpp>

#include <SFML/System/String.hpp>
#include <Genode/Graphics/Sprite.hpp>
#include <Genode/Graphics/Animation.hpp>

#include <tuple>
#include <unordered_map>
#include <memory>

class Item
{
public:
    Item();

    unsigned int GetId() const;
    void SetId(unsigned int id);

    Equipment::Type GetType() const;
    void SetType(Equipment::Type type);

    Planet::Planet GetPlanet() const;
    void SetPlanet(Planet::Planet planet);

    Character::Gender GetGender() const;
    void SetGender(Character::Gender gender);

    bool IsNew() const;
    void SetIsNew(bool isNew);

    Shop::Currency GetPriceCurrency() const;
    void SetPriceCurrency(Shop::Currency currency);

    unsigned int GetPrice() const;
    void SetPrice(unsigned int price);

    const sf::String &GetName() const;
    void SetName(const sf::String &name);

    const sf::String &GetDescription() const;
    void SetDescription(const sf::String &description);

    const Gx::Sprite *GetSmallPreview() const;
    void SetSmallPreview(Gx::ResourcePtr<Gx::Sprite> smallPreview);

    const Gx::Sprite *GetLargePreview() const;
    void SetLargePreview(Gx::ResourcePtr<Gx::Sprite> largePreview);

    void SetRenderableItem(Character::Gender gender, Equipment::RenderType renderType, Equipment::Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation);
    Gx::Animation *GetRenderableItem(Character::Gender gender, Equipment::RenderType renderType, Equipment::Instrument instrument) const;

    void ResetRenderables();

private:
    using RenderableKey = std::tuple<Character::Gender, Equipment::RenderType, Equipment::Instrument>;

    unsigned int      m_id;
    Equipment::Type   m_type;
    Planet::Planet    m_planet;
    Character::Gender m_gender;
    bool              m_isNew;
    Shop::Currency    m_priceCurrency;
    unsigned int      m_price;
    sf::String        m_name, m_description;

    Gx::ResourcePtr<Gx::Sprite> m_smallPreview, m_largePreview;
    std::map<RenderableKey, Gx::ResourcePtr<Gx::Animation>> m_renderables;
};

#endif
