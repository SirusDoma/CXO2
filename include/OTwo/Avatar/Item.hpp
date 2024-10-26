#pragma once

#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Serializable.g.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/IO/Resource.hpp>

#include <SFML/System/String.hpp>

#include <tuple>
#include <unordered_map>
#include <memory>
#include <map>

class Item final
{
public:
    Item() = default;

    unsigned int GetID() const;
    void SetID(unsigned int id);

    EquipmentType GetType() const;
    void SetType(EquipmentType type);

    bool IsEquipable() const;

    Instrument GetInstrument() const;

    Planet GetOrigin() const;
    void SetOrigin(Planet planet);

    Gender GetGender() const;
    void SetGender(Gender gender);

    bool IsNew() const;
    void SetIsNew(bool isNew);

    const sf::String& GetName() const;
    void SetName(const sf::String& name);

    const sf::String& GetDescription() const;
    void SetDescription(const sf::String& description);

    bool IsBuyableWith(const Currency& currency) const;

    unsigned int GetPrice(const Currency& currency) const;
    void SetPrice(const Currency& currency, unsigned int price);

    const Gx::Sprite& GetSmallThumbnail() const;
    void SetSmallThumbnail(Gx::Sprite&& thumbnail);

    const Gx::Sprite& GetLargeThumbnail() const;
    void SetLargeThumbnail(Gx::Sprite&& thumbnail);

    void SetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument, Gx::Animation&& animation);
    Gx::Animation* GetRenderableItem(Gender gender, RenderPart renderType, Instrument instrument);
    std::vector<Gx::Animation*> GetRenderables();

    void ResetRenderables();

private:
    using RenderableKey = std::tuple<Gender, RenderPart, Instrument>;
    using PriceMap      = std::unordered_map<Currency, unsigned int>;

    unsigned int     m_id;
    EquipmentType    m_type;
    Planet           m_origin;
    Gender           m_gender;
    bool             m_isNew;
    sf::String       m_name, m_description;
    mutable PriceMap m_prices;

    Gx::Sprite m_smallThumbnail, m_largeThumbnail;
    std::map<RenderableKey, Gx::Animation> m_renderables;
};
