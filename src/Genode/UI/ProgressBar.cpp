#include <Genode/UI/ProgressBar.hpp>

namespace Gx
{
    ProgressBar::ProgressBar() :
        m_vertices(sf::TriangleStrip, 4),
        m_texture(),
        m_texCoords(),
        m_orientation(Orientation::Horizontal),
        m_value(0),
        m_maximum(100.0f)
    {

    }

    ProgressBar::ProgressBar(const sf::Texture &texture) :
        ProgressBar(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y))
    {

    }

    ProgressBar::ProgressBar(const sf::Texture &texture, const sf::IntRect &texCoords) :
        ProgressBar()
    {
        SetTexture(texture);
        SetTexCoords(texCoords);
    }

    sf::FloatRect ProgressBar::GetLocalBounds() const
    {
        return sf::FloatRect(0, 0, m_texCoords.width, m_texCoords.height);
    }

    const sf::Texture *ProgressBar::GetTexture() const
    {
        return m_texture;
    }

    void ProgressBar::SetTexture(const sf::Texture &texture)
    {
        m_texture = &texture;
    }

    const sf::IntRect& ProgressBar::GetTexCoords() const
    {
        return m_texCoords;
    }

    void ProgressBar::SetTexCoords(const sf::IntRect &texCoords)
    {
        if (texCoords != m_texCoords)
        {
            m_texCoords = texCoords;
            Invalidate();
        }
    }

    const sf::Color &ProgressBar::GetColor() const
    {
        return m_vertices[0].color;
    }

    void ProgressBar::SetColor(const sf::Color &color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }

    const ProgressBar::Orientation ProgressBar::GetOrientation() const
    {
        return m_orientation;
    }

    void ProgressBar::SetOrientation(const Orientation &orientation)
    {
        if (m_orientation != orientation)
        {
            m_orientation = orientation;
            Invalidate();
        }
    }

    float ProgressBar::GetMaximumValue() const
    {
        return m_maximum;
    }

    void ProgressBar::SetMaximumValue(float max)
    {
        if (m_maximum != max)
        {
            m_maximum = max;
            Invalidate();
        }
    }

    float ProgressBar::GetValue() const
    {
        return m_value;
    }

    void ProgressBar::SetValue(float value)
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

    void ProgressBar::Update(double delta)
    {
        Control::Update(delta);
    }

    sf::RenderStates ProgressBar::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (m_texture)
        {
            states.transform *= GetTransform();
            states.texture    = m_texture;
            target.draw(m_vertices, states);
        }

        return Control::Render(target, states);
    }

    void ProgressBar::Invalidate()
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

        m_vertices[0] = sf::Vertex(sf::Vector2f(x, y), color, sf::Vector2f(left, top));
        m_vertices[1] = sf::Vertex(sf::Vector2f(x, h), color, sf::Vector2f(left, bottom));
        m_vertices[2] = sf::Vertex(sf::Vector2f(w, y), color, sf::Vector2f(right, top));
        m_vertices[3] = sf::Vertex(sf::Vector2f(w, h), color, sf::Vector2f(right, bottom));
    }
}
