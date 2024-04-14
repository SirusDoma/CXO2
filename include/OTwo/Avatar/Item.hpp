#ifndef O2JAM_AVATAR_ITEM_HPP
#define O2JAM_AVATAR_ITEM_HPP

#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>
#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Shop.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/IO/Resource.hpp>

#include <SFML/System/String.hpp>

#include <tuple>
#include <unordered_map>
#include <memory>
#include <map>

class Item
{
public:
    Item() = default;

    unsigned int GetID() const;
    void SetID(unsigned int id);

    EquipmentType GetType() const;
    void SetType(EquipmentType type);

    Instrument GetInstrument() const;

    Planet GetOrigin() const;
    void SetOrigin(Planet planet);

    Gender GetGender() const;
    void SetGender(Gender gender);

    bool IsNew() const;
    void SetIsNew(bool isNew);

    const sf::String &GetName() const;
    void SetName(const sf::String &name);

    const sf::String &GetDescription() const;
    void SetDescription(const sf::String &description);

    bool IsBuyableWith(const Currency &currency) const;

    unsigned int GetPrice(const Currency &currency) const;
    void SetPrice(const Currency &currency, unsigned int price);

    const Gx::Sprite *GetSmallPreview() const;
    void SetSmallPreview(Gx::ResourcePtr<Gx::Sprite> smallPreview);

    const Gx::Sprite *GetLargePreview() const;
    void SetLargePreview(Gx::ResourcePtr<Gx::Sprite> largePreview);

    void SetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument, Gx::ResourcePtr<Gx::Animation> animation);
    Gx::Animation *GetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument) const;
    std::vector<Gx::Animation*> GetRenderables() const;

    void ResetRenderables();

private:
    using RenderableKey = std::tuple<Gender, RenderPart, Instrument>;
    using PriceMap      = std::unordered_map<Currency, unsigned int>;

    unsigned int     m_id;
    EquipmentType    m_type;
    Planet       m_origin;
    Gender           m_gender;
    bool             m_isNew;
    unsigned int     m_price;
    sf::String       m_name, m_description;
    mutable PriceMap m_prices;

    Gx::ResourcePtr<Gx::Sprite> m_smallPreview, m_largePreview;
    std::map<RenderableKey, Gx::ResourcePtr<Gx::Animation>> m_renderables;
};

#endif
