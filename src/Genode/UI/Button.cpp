#include <Genode/UI/Button.hpp>

namespace Gx
{
    Button::Button() :
        m_sprite()
    {
    }

    Button::Button(const sf::Texture &texture) :
        m_sprite(texture)
    {
    }

    Button::Button(const sf::Texture &texture, const sf::IntRect &rectangle) :
        m_sprite(texture, rectangle)
    {
    }

    Sprite *Button::GetSprite() const
    {
        return &m_sprite;
    }

    const sf::Texture *Button::GetTexture() const
    {
        return m_sprite.GetTexture();
    }

    void Button::SetTexture(const sf::Texture &texture)
    {
        m_sprite.SetTexture(texture);
    }

    const sf::FloatRect Button::GetLocalBounds() const
    {
        // Use frame for active state first before looking for other frames
        auto bounds = m_stateData[GetControlState()].GetLocalBounds();
        if (bounds.width > 0 && bounds.height > 0)
            return bounds;

        // There's no frame for active state, look for valid frame
        for (auto [_, frame] : m_stateData)
        {
            bounds = frame.GetLocalBounds();
            if (bounds.width > 0 && bounds.height > 0)
                return bounds;
        }

        return sf::FloatRect();
    }

    const Sprite Button::GetStateFrame(Button::State state) const
    {
        return m_stateData[state];
    }

    void Button::SetStateFrame(Button::State state, sf::IntRect texCoords, sf::Color color)
    {
        auto sprite = Gx::Sprite();
        sprite.SetTexCoords(texCoords);
        sprite.SetColor(color);

        SetStateFrame(state, sprite);
    }

    void Button::SetStateFrame(Button::State state, const Sprite &sprite)
    {
        m_stateData[state] = sprite;
        Invalidate();
    }

    sf::RenderStates Button::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        auto frame = m_stateData[GetControlState()];
        states.transform *= GetTransform();
        states.transform *= frame.GetTransform();
        target.draw(m_sprite, states);

        return RenderableContainer::Render(target, states);
    }

    void Button::Invalidate()
    {
        auto frame = m_stateData[GetControlState()];

        m_sprite.SetColor(frame.GetColor());
        m_sprite.SetTexCoords(frame.GetTexCoords());
    }
}