#include <CXO2/UI/Components/Marquee.hpp>

namespace Cx
{
    Marquee::Marquee() :
        m_bounds(),
        m_speed()
    {
    }

    Marquee::Marquee(const Marquee& copy) :
        Label(copy),
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
    }

    Gx::RenderStates Marquee::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        EnsureGeometryUpdate();

        if (m_unclippedStale)
        {
            m_unclippedVertices        = GetVertexData();
            m_unclippedOutlineVertices = GetOutlineVertexData();
            m_unclippedStale           = false;
        }
        else
        {
            GetVertexData()        = m_unclippedVertices;
            GetOutlineVertexData() = m_unclippedOutlineVertices;
        }

        ClipQuads(sf::FloatRect(GetOrigin() - GetPosition(), m_bounds.size));

        auto textStates = states;
        textStates.transform.translate(m_bounds.position);

        return Text::Render(surface, textStates);
    }

    void Marquee::OnGeometryUpdated() const
    {
        Label::OnGeometryUpdated();
        m_unclippedStale = true;
    }
}
