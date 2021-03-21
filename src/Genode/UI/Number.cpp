#include <Genode/UI/Number.hpp>

namespace Gx
{
    Number::Number()
    {
    }

    Number::Number(const sf::Texture& texture) :
        Number(std::make_shared<sf::Texture>(texture))
    {
    }

    Number::Number(TextureHandle texture) :
        m_alignment(Alignment::Center),
        m_vertices(sf::Triangles, 6 * 10),
        m_texCoords(),
        m_digitCount(),
        m_width(),
        m_height(),
        m_spacing(),
        m_needUpdate(true)
    {
        SetTexture(texture);
        SetValue(0);
    }

    const sf::FloatRect Number::GetLocalBounds() const
    {
        return sf::FloatRect(0, 0, m_width, m_height);
    }

    const sf::Texture* Number::GetTexture() const
    {
        return m_texture.get();
    }

    void Number::SetTexture(const sf::Texture& texture)
    {
        m_texture = std::make_shared<sf::Texture>(texture);
    }

    void Number::SetTexture(TextureHandle texture)
    {
        m_texture = texture;
    }

    const sf::Color & Number::GetColor() const
    {
        return m_vertices[0].color;
    }

    void Number::SetColor(const sf::Color &color)
    {
        for (unsigned int i = 0; i < m_vertices.getVertexCount(); i++)
            m_vertices[i].color = color;
    }

    Number::Alignment Number::GetAlignment() const
    {
        return m_alignment;
    }

    void Number::SetAlignment(Alignment alignment)
    {
        m_alignment = alignment;
        m_needUpdate = true;
    }

    float Number::GetLetterSpacing() const
    {
        return m_spacing;
    }

    void Number::SetLetterSpacing(float spacing)
    {
        m_spacing = spacing;
        m_needUpdate = true;
    }

    int Number::GetDigitCount() const
    {
        return m_digitCount;
    }

    void Number::SetDigitCount(int count)
    {
        m_digitCount = count;
        m_needUpdate = true;
    }

    void Number::SetDigitFrame(unsigned int digit, sf::IntRect texCoords)
    {
        m_texCoords[digit % 10] = texCoords;
        m_needUpdate = true;
    }

    void Number::SetDigitsSize(sf::Vector2u size)
    {
        m_texCoords.clear();
        unsigned int current = 0;
        for (int i = 1; i <= 10; i++)
        {
            SetDigitFrame(i, sf::IntRect(current, 0, size.x, size.y));
            current += size.x;
        }

        m_needUpdate = true;
    }

    unsigned int Number::GetValue() const
    {
        return m_value;
    }

    void Number::SetValue(unsigned int value)
    {
        m_value = value;
        m_needUpdate = true;
    }

    void Number::Update(double delta)
    {
        if (m_needUpdate)
            Invalidate();

        Control::Update(delta);
    }

    sf::RenderStates Number::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= GetTransform();
        states.texture    = m_texture.get();
        target.draw(m_vertices, states);

        return Control::Render(target, states);
    }

    void Number::Invalidate()
    {
        m_width = 0;
        auto color = GetColor();
        m_vertices.clear();

        unsigned digit = 0, digitCount = 0, leadingCount = 0, value = m_value, width = m_texture->getSize().x;
        if (value > 0)
        {
            while (value > 0)
            {
                value /= 10;
                digitCount++;
            }
        }
        else
            digitCount = 1;

        if (m_digitCount > 0 && digitCount < m_digitCount)
            leadingCount = m_digitCount - digitCount;

        digitCount += leadingCount;
        auto position = sf::Vector2f();

        for (int i = 0, j = digitCount - 1; i < digitCount; i++, j--)
        {
            bool isLeading = false;
            if (leadingCount > 0)
                isLeading = i >= leadingCount;

            if (isLeading)
                digit = 0;
            else
                digit = value % 10;

            auto texCoords = m_texCoords[digit];
            switch (m_alignment)
            {
                case Alignment::Left:   position = sf::Vector2f((width * m_spacing) * j, 0); break;
                case Alignment::Center: position = sf::Vector2f((width * m_spacing) * j - digitCount * (m_width * m_spacing) / 2, 0); break;
                case Alignment::Right:  position = sf::Vector2f((width * m_spacing) * j - digitCount * (m_width * m_spacing), 0); break;
            }

            float x = position.x;
            float y = position.y;
            float w = position.x + texCoords.width;
            float h = position.y + texCoords.height;

            float left   = texCoords.left;
            float top    = texCoords.top;
            float right  = texCoords.left + texCoords.width;
            float bottom = texCoords.top  + texCoords.height;

            int index = i * 6;
            m_vertices[index + 0] = sf::Vertex(sf::Vector2f(x, y), color, sf::Vector2f(left , top));
            m_vertices[index + 1] = sf::Vertex(sf::Vector2f(w, y), color, sf::Vector2f(right , top));
            m_vertices[index + 2] = sf::Vertex(sf::Vector2f(x, h), color, sf::Vector2f(left , bottom));
            m_vertices[index + 3] = sf::Vertex(sf::Vector2f(x, h), color, sf::Vector2f(left , bottom));
            m_vertices[index + 4] = sf::Vertex(sf::Vector2f(w, y), color, sf::Vector2f(right , top));
            m_vertices[index + 5] = sf::Vertex(sf::Vector2f(w, h), color, sf::Vector2f(right , bottom));

            if (!isLeading)
                value /= 10;
        }

        m_needUpdate = false;
    }
}