#include <OTwo/UI/Common/Marquee.hpp>

#include <Genode/System/Exception.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

Marquee::Marquee() :
    m_renderTexture(std::make_unique<sf::RenderTexture>()),
    m_speed()
{
}

Marquee::Marquee(const Marquee &copy) :
    Gx::Label(copy),
    m_renderTexture(std::move(copy.m_renderTexture)),
    m_bounds(copy.m_bounds),
    m_speed(copy.m_speed)
{
}

sf::FloatRect Marquee::GetLocalBounds() const
{
    return m_bounds;
}

void Marquee::SetLocalBounds(const sf::FloatRect &bounds)
{
    m_bounds = bounds;

    if (!m_renderTexture->create(sf::Vector2u(static_cast<unsigned int>(m_bounds.width), static_cast<unsigned int>(m_bounds.height))))
        throw Gx::Exception("Failed to create render texture.");

    m_renderTexture->setSmooth(true);

    m_sprite.SetTexture(m_renderTexture->getTexture(), true);
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

void Marquee::Update(const double delta)
{
    Label::Update(delta);

    const auto velocity = sf::Vector2f(m_speed, 0.f) * static_cast<float>(delta / 1000.f);
    const auto position = GetPosition() - velocity;
    SetPosition(position);

    const auto textBounds = Label::GetLocalBounds();
    if (const float x = GetPosition().x + textBounds.width; x <= 0)
        SetPosition(sf::Vector2f(m_bounds.width, GetPosition().y));

    m_renderTexture->clear(sf::Color::Transparent);
    {
        auto adapter = Gx::RenderTargetAdapter(*m_renderTexture);
        Text::Render(adapter, Gx::RenderStates::Default);
    }
    m_renderTexture->display();
}

Gx::RenderStates Marquee::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    auto transform = GetTransform();
    states.transform *= transform.translate(-GetPosition());
    surface.Render(m_sprite, states);

    return states;
}

void Marquee::Invalidate()
{
    Label::Invalidate();
}
