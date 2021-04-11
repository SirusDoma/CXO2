#include <O2/States/Components/Room/Marquee.hpp>

const sf::FloatRect Marquee::GetLocalBounds() const
{
    return m_bounds;
}

void Marquee::SetLocalBounds(const sf::FloatRect &bounds)
{
    m_bounds = bounds;

    m_renderTexture.create(m_bounds.width, m_bounds.height);
    m_renderTexture.setSmooth(true);

    m_sprite.SetTexture(m_renderTexture.getTexture(), true);
    m_sprite.SetPosition(sf::Vector2f(m_bounds.left, m_bounds.top));

    SetPosition(sf::Vector2f(m_bounds.width, GetPosition().y));
}

double Marquee::GetSpeed() const
{
    return m_speed;
}

void Marquee::SetSpeed(double speed)
{
    m_speed = speed;
}

void Marquee::Update(double delta)
{
    Label::Update(delta);

    auto velocity = sf::Vector2f(m_speed, 0.f) * static_cast<float>(delta / 1000.f);
    SetPosition(GetPosition() - velocity);

    auto textBounds = Label::GetLocalBounds();
    float x = GetPosition().x + textBounds.width;
    if (x <= 0)
        SetPosition(sf::Vector2f(m_bounds.width, GetPosition().y));

    m_renderTexture.clear(sf::Color::Transparent);
    {
        Text::Render(m_renderTexture, sf::RenderStates::Default);
        Text::Render(m_renderTexture, sf::RenderStates::Default);
    }
    m_renderTexture.display();
}

sf::RenderStates Marquee::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    auto transform = GetTransform();
    states.transform *= transform.translate(-GetPosition());
    target.draw(m_sprite, states);

    return states;
}

void Marquee::Invalidate()
{
    Label::Invalidate();
}
