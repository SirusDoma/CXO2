#ifndef O2JAM_CHARACTER_ITEM_HPP
#define O2JAM_CHARACTER_ITEM_HPP

#include <O2/Data/Character.hpp>
#include <O2/Data/Equipment.hpp>
#include <O2/Data/Planet.hpp>
#include <O2/Data/Shop.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/System/String.hpp>

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

    Planet::Planet GetOrigin() const;
    void SetOrigin(Planet::Planet planet);

    Character::Gender GetGender() const;
    void SetGender(Character::Gender gender);

    bool IsNew() const;
    void SetIsNew(bool isNew);

    const sf::String &GetName() const;
    void SetName(const sf::String &name);

    const sf::String &GetDescription() const;
    void SetDescription(const sf::String &description);

    bool IsBuyableWith(const Shop::Currency &currency) const;

    unsigned int GetPrice(const Shop::Currency &currency) const;
    void SetPrice(const Shop::Currency &currency, unsigned int price);

    const Gx::Sprite *GetSmallPreview() const;
    void SetSmallPreview(Gx::ResourcePtr<Gx::Sprite> smallPreview);

    const Gx::Sprite *GetLargePreview() const;
    void SetLargePreview(Gx::ResourcePtr<Gx::Sprite> largePreview);

    void SetRenderableItem(Character::Gender gender, Equipment::RenderPart renderType, Equipment::Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation);
    Gx::Animation *GetRenderableItem(Character::Gender gender, Equipment::RenderPart renderType, Equipment::Instrument instrument) const;
    std::vector<Gx::Animation*> GetRenderables() const;

    void ResetRenderables();

private:
    using RenderableKey = std::tuple<Character::Gender, Equipment::RenderPart, Equipment::Instrument>;

    unsigned int      m_id;
    Equipment::Type   m_type;
    Planet::Planet    m_origin;
    Character::Gender m_gender;
    bool              m_isNew;
    unsigned int      m_price;
    sf::String        m_name, m_description;
    mutable std::map<Shop::Currency, unsigned int> m_prices;

    Gx::ResourcePtr<Gx::Sprite> m_smallPreview, m_largePreview;
    std::map<RenderableKey, Gx::ResourcePtr<Gx::Animation>> m_renderables;
};

#endif
