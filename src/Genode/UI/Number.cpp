#include <Genode/UI/Number.hpp>
#include <stack>

namespace Gx
{
    Number::Number() :
        m_vertices(sf::PrimitiveType::TriangleStrip, 6 * 10),
        m_texture(),
        m_blendMode(BlendMode::Auto),
        m_alignment(Alignment::Left),
        m_texCoords(),
        m_digitCount(),
        m_width(),
        m_height(),
        m_kerning(),
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

    sf::FloatRect Number::GetLocalBounds() const
    {
        return sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(m_width, m_height));
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

    float Number::GetKerning() const
    {
        return m_kerning;
    }

    void Number::SetKerning(const float value)
    {
        m_kerning = value;
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
            SetDigitFrame(i, sf::IntRect(sf::Vector2i(current, 0), sf::Vector2i(size.x, size.y)));
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

    Number::Alignment Number::GetAlignment() const
    {
        return m_alignment;
    }

    void Number::SetAlignment(const Alignment alignment)
    {
        m_alignment = alignment;
        m_needUpdate = true;
    }

    BlendMode Number::GetBlendMode() const
    {
        return m_blendMode;
    }

    void Number::SetBlendMode(const Gx::BlendMode blendMode)
    {
        m_blendMode = blendMode;
    }

    void Number::Update(const double delta)
    {
        if (m_needUpdate)
            Invalidate();

        Control::Update(delta);
    }

    RenderStates Number::Render(RenderSurface &surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();
        states.texture    = m_texture;
        switch (m_blendMode)
        {
            case BlendMode::Alpha:          states.blendMode = sf::BlendAlpha;    break;
            case BlendMode::Additive:       states.blendMode = sf::BlendAdd;      break;
            case BlendMode::Multiplicative: states.blendMode = sf::BlendMultiply; break;
            case BlendMode::Min:            states.blendMode = sf::BlendMin;      break;
            case BlendMode::Max:            states.blendMode = sf::BlendMax;      break;
            case BlendMode::None:           states.blendMode = sf::BlendNone;     break;
            case BlendMode::Auto:                                                 break;
        }
        surface.Render(m_vertices, states);

        return Control::Render(surface, states);
    }

    void Number::Invalidate()
    {
        auto color = GetColor();
        m_vertices = sf::VertexArray(sf::PrimitiveType::Triangles, 6 * 10);
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
                m_width += m_kerning;

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
                position += sf::Vector2f(texCoords.width + m_kerning, 0);

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
            m_vertices[index + 0] = {sf::Vector2f(x, y), color, sf::Vector2f(left , top)};
            m_vertices[index + 1] = {sf::Vector2f(w, y), color, sf::Vector2f(right , top)};
            m_vertices[index + 2] = {sf::Vector2f(x, h), color, sf::Vector2f(left , bottom)};
            m_vertices[index + 3] = {sf::Vector2f(x, h), color, sf::Vector2f(left , bottom)};
            m_vertices[index + 4] = {sf::Vector2f(w, y), color, sf::Vector2f(right , top)};
            m_vertices[index + 5] = {sf::Vector2f(w, h), color, sf::Vector2f(right , bottom)};

            m_height = m_height < texCoords.height ? texCoords.height : m_height;
        }

        if (m_alignment != Alignment::None)
        {
            if (m_alignment == Alignment::Left)
                SetOrigin(m_width / 2.f, 0);
            else if (m_alignment == Alignment::Center)
                SetOrigin(m_width / 2.f, m_height / 2.f);
            else if (m_alignment == Alignment::Right)
                SetOrigin(m_width, 0);
        }

        m_needUpdate = false;
    }
}
