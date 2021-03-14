#include <Genode/UI/Button.hpp>

namespace Gx
{
    Button::Button() :
        m_sprite(),
        m_state(Button::State::NORMAL)
    {
    }

    Button::Button(const sf::Texture &texture) :
        m_sprite(texture),
        m_state(Button::State::NORMAL)
    {
    }

    Button::Button(const sf::Texture &texture, const sf::IntRect &rectangle) :
        m_sprite(texture, rectangle),
        m_state(Button::State::NORMAL)
    {
    }

    Button::Button(TextureHandle texture) :
        m_sprite(texture),
        m_state(Button::State::NORMAL)
    {
    }

    Button::Button(TextureHandle texture, const sf::IntRect &rectangle) :
        m_sprite(texture, rectangle),
        m_state(Button::State::NORMAL)
    {
    }

    void Button::SetClickCallback(std::function<void()> callback)
    {
        m_click = callback;
    }

    Sprite *Button::GetSprite() const
    {
        return &m_sprite;
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
    }

    const Button::State Button::GetState() const
    {
        return m_state;
    }

    void Button::SetState(const Button::State &state)
    {
        m_state = state;
    }

    void Button::Update(double delta)
    {
    }

    sf::RenderStates Button::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        auto frame = m_stateData[m_state];
        m_sprite.SetColor(frame.GetColor());
        m_sprite.SetTexCoords(frame.GetTexCoords());

        states.transform *= GetTransform() * frame.GetTransform();
        target.draw(m_sprite, states);

        return states;
    }

    void Button::OnMouseMove(sf::Event::MouseMoveEvent ev)
    {
        if (!IsEnabled() || m_state == Button::State::PRESSED)
            return;

        bool intersect = IsIntersect(sf::Vector2f(ev.x, ev.y));
        if (intersect && m_state == Button::State::NORMAL)
            m_state = Button::State::HOVER;
        else if (!intersect && m_state == Button::State::HOVER)
            m_state = Button::State::NORMAL;
    }

    void Button::OnMouseButtonClick(sf::Event::MouseButtonEvent ev)
    {
        if (IsEnabled() && m_state == Button::State::HOVER && IsIntersect(sf::Vector2f(ev.x, ev.y)))
            m_state = Button::State::PRESSED;
    }

    void Button::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled() || m_state != Button::State::PRESSED)
            return;

        m_state = Button::State::NORMAL;
        if (IsIntersect(sf::Vector2f(ev.x, ev.y)))
        {
            m_state = Button::State::HOVER;
            if (m_click)
                m_click();
        }
    }

    bool Button::IsIntersect(sf::Vector2f ev)
    {
        auto position = GetPosition();
        sf::FloatRect bounds = sf::FloatRect();
        for (auto state : { Button::State::NORMAL, Button::State::HOVER, Button::State::PRESSED })
        {
            bounds = m_stateData[state].GetLocalBounds();
            if (bounds.width != 0 && bounds.height != 0)
                break;
        }

        bounds.top  = position.y;
        bounds.left = position.x;
        return bounds.contains(ev.x, ev.y);
    }
}