#include <Genode/UI/Number.hpp>
#include <stack>

namespace Gx
{
    Number::Number() :
        m_vertices(sf::TriangleStrip, 6 * 10),
        m_texture(),
        m_texCoords(),
        m_digitCount(),
        m_width(),
        m_height(),
        m_spacing(),
        m_value(),
        m_needUpdate(true)
    {
    }

    Number::Number(const sf::Texture& texture) :
        Number()
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
        return m_texture;
    }

    void Number::SetTexture(const sf::Texture& texture)
    {
        m_texture = &texture;
        m_needUpdate = true;
    }

    const sf::Color& Number::GetColor() const
    {
        return m_vertices[0].color;
    }

    void Number::SetColor(const sf::Color &color)
    {
        for (unsigned int i = 0; i < m_vertices.getVertexCount(); i++)
            m_vertices[i].color = color;
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
        if (size == sf::Vector2u())
            return;

        m_texCoords.clear();
        unsigned int current = 0;
        for (int i = 0; i < 10; i++)
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

    void Number::Align(Number::Alignment alignment)
    {
        m_alignment = alignment;
        Invalidate();
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
        states.texture    = m_texture;
        target.draw(m_vertices, states);

        return Control::Render(target, states);
    }

    void Number::Invalidate()
    {
        auto color = GetColor();
        m_vertices = sf::VertexArray(sf::Triangles, 6 * 10);
        m_width    = 0;

        unsigned digit = 0, digitCount = 0, leadingCount = 0, value = m_value;
        if (value > 0)
        {
            // TODO: Merge this loop
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
        value = m_value;

        std::stack<unsigned int> digits = std::stack<unsigned int>();
        for (int d = 0; d < digitCount; d++)
        {
            if (d > 0)
                m_width += m_spacing;

            bool isLeading = leadingCount > 0 && d > digitCount - leadingCount;
            if (isLeading)
                digit = 0;
            else
                digit = value % 10;

            if (!isLeading)
                value /= 10;

            auto texCoords = m_texCoords[digit];
            m_width += texCoords.width;

            digits.push(digit);
        }

        auto position = sf::Vector2f();
        auto size = digits.size();
        auto texCoords = sf::IntRect();
        for (unsigned int i = 0; i < size; i++)
        {
            digit = digits.top();
            digits.pop();

            if (i > 0)
                position += sf::Vector2f(texCoords.width + m_spacing, 0);

            texCoords = m_texCoords[digit];
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

            m_height = m_height < texCoords.height ? texCoords.height : m_height;
        }

        if (m_alignment != None)
        {
            if (m_alignment == Left)
                SetOrigin(m_width / 2.f, 0);
            else if (m_alignment == Center)
                SetOrigin(m_width / 2.f, m_height / 2.f);
            else if (m_alignment == Right)
                SetOrigin(m_width, 0);
        }

        m_needUpdate = false;
    }
}