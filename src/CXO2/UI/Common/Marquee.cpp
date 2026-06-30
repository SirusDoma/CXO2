#include <CXO2/UI/Common/Marquee.hpp>

#include <Genode/System/Exception.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

namespace Cx
{
    Marquee::Marquee() :
        m_renderTexture(std::make_unique<sf::RenderTexture>()),
        m_speed()
    {
    }

    Marquee::Marquee(const Marquee& copy) :
        Label(copy),
        m_renderTexture(std::move(copy.m_renderTexture)),
        m_bounds(copy.m_bounds),
        m_speed(copy.m_speed)
    {
    }

    sf::FloatRect Marquee::GetLocalBounds() const
    {
        return m_bounds;
    }

    void Marquee::SetLocalBounds(const sf::FloatRect& bounds)
    {
        m_bounds = bounds;

        if (!m_renderTexture->resize(sf::Vector2u(static_cast<unsigned int>(m_bounds.size.x), static_cast<unsigned int>(m_bounds.size.y))))
            throw Gx::Exception("Failed to create render texture");

        m_renderTexture->setSmooth(true);

        m_sprite.SetTexture(m_renderTexture->getTexture(), true);
        m_sprite.SetPosition(sf::Vector2f(m_bounds.position.x, m_bounds.position.y));

        SetPosition(sf::Vector2f(m_bounds.size.x, GetPosition().y));
    }

    double Marquee::GetSpeed() const
    {
        return m_speed;
    }

    void Marquee::SetSpeed(double speed)
    {
        m_speed = speed;
    }

    void Marquee::Update(const sf::Time& delta)
    {
        Label::Update(delta);

        const auto velocity = sf::Vector2f(m_speed, 0.f) * delta.asSeconds();
        const auto position = GetPosition() - velocity;
        SetPosition(position);

        const auto textBounds = Label::GetLocalBounds();
        if (const float x = GetPosition().x + textBounds.size.x; x <= 0)
            SetPosition(sf::Vector2f(m_bounds.size.x, GetPosition().y));

        m_renderTexture->clear(sf::Color::Transparent);
        {
            auto adapter = Gx::RenderSurfaceAdaptor(*m_renderTexture);
            Text::Render(adapter, Gx::RenderStates::Default);
        }
        m_renderTexture->display();
    }

    Gx::RenderStates Marquee::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
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
}
