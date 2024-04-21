#include <Genode/UI/Gauge.hpp>

namespace Gx
{
    Gauge::Gauge() :
        m_vertices(sf::PrimitiveType::TriangleStrip, 4),
        m_texture(),
        m_texCoords(),
        m_orientation(Orientation::Horizontal),
        m_value(0),
        m_maximum(100.0f)
    {

    }

    Gauge::Gauge(const sf::Texture &texture) :
        Gauge(texture, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)))
    {
    }

    Gauge::Gauge(const sf::Texture &texture, const sf::IntRect &texCoords) :
        Gauge()
    {
        SetTexture(texture);
        SetTexCoords(texCoords);
    }

    sf::FloatRect Gauge::GetLocalBounds() const
    {
        return sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(m_texCoords.width, m_texCoords.height));
    }

    const sf::Texture *Gauge::GetTexture() const
    {
        return m_texture;
    }

    void Gauge::SetTexture(const sf::Texture &texture)
    {
        m_texture = &texture;
    }

    const sf::IntRect& Gauge::GetTexCoords() const
    {
        return m_texCoords;
    }

    void Gauge::SetTexCoords(const sf::IntRect &texCoords)
    {
        if (texCoords != m_texCoords)
        {
            m_texCoords = texCoords;
            Invalidate();
        }
    }

    const sf::Color &Gauge::GetColor() const
    {
        return m_vertices[0].color;
    }

    void Gauge::SetColor(const sf::Color &color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }

    const Gauge::Orientation Gauge::GetOrientation() const
    {
        return m_orientation;
    }

    void Gauge::SetOrientation(const Orientation &orientation)
    {
        if (m_orientation != orientation)
        {
            m_orientation = orientation;
            Invalidate();
        }
    }

    float Gauge::GetMaximumValue() const
    {
        return m_maximum;
    }

    void Gauge::SetMaximumValue(const float max)
    {
        if (m_maximum != max)
        {
            m_maximum = max;
            Invalidate();
        }
    }

    float Gauge::GetValue() const
    {
        return m_value;
    }

    void Gauge::SetValue(float value)
    {
        if (value > m_maximum)
            value = m_maximum;

        if (value < 0)
            value = 0;

        if (m_value != value)
        {
            m_value = value;
            Invalidate();
        }
    }

    void Gauge::Update(const double delta)
    {
        Control::Update(delta);
    }

    RenderStates Gauge::Render(sf::RenderTarget &target, RenderStates states) const
    {
        if (m_texture)
        {
            states.transform *= GetTransform();
            states.texture    = m_texture;
            target.draw(m_vertices, states);
        }

        return Control::Render(target, states);
    }

    void Gauge::Invalidate()
    {
        auto color  = GetColor();
        auto bounds = GetLocalBounds();
        int x = bounds.left;
        int y = bounds.top;
        float w = static_cast<float>(bounds.width);
        float h = static_cast<float>(bounds.height);

        float left   = static_cast<float>(m_texCoords.left);
        float right  = left + w;
        float top    = static_cast<float>(m_texCoords.top);
        float bottom = top + h;

        if (m_value < 100)
        {
            if (m_orientation == Orientation::Horizontal)
            {
                float progress = static_cast<float>(bounds.width) * (m_value / 100.f);
                w     = progress;
                right = left + w;
            }
            else
            {
                float progress = static_cast<float>(bounds.height) * (m_value / 100.f);
                y   = progress;
                top = progress;
            }
        }

        m_vertices[0] = {sf::Vector2f(x, y), color, sf::Vector2f(left, top)};
        m_vertices[1] = {sf::Vector2f(x, h), color, sf::Vector2f(left, bottom)};
        m_vertices[2] = {sf::Vector2f(w, y), color, sf::Vector2f(right, top)};
        m_vertices[3] = {sf::Vector2f(w, h), color, sf::Vector2f(right, bottom)};
    }
}
