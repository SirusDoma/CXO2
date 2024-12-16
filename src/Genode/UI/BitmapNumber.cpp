#include <Genode/UI/BitmapNumber.hpp>
#include <stack>

namespace Gx
{
    BitmapNumber::BitmapNumber() :
        m_vertices(sf::PrimitiveType::TriangleStrip, 6 * 10),
        m_texture(),
        m_blendMode(BlendMode::Auto),
        m_alignment(Alignment::Left),
        m_state(Animation::AnimationState::Initial),
        m_value(),
        m_digitCount(),
        m_width(),
        m_height(),
        m_kerning(),
        m_needUpdate(true),
        m_texCoords()
    {
    }

    BitmapNumber::BitmapNumber(const sf::Texture& texture) :
        BitmapNumber()
    {
        SetTexture(texture);
        SetValue(0);
    }

    sf::FloatRect BitmapNumber::GetLocalBounds() const
    {
        if (m_needUpdate)
        {
            // Hack
            const auto self = const_cast<BitmapNumber*>(this);
            self->Invalidate();
        }

        return { {0, 0}, {m_width, m_height} };
    }

    const sf::Texture* BitmapNumber::GetTexture() const
    {
        return m_texture;
    }

    void BitmapNumber::SetTexture(const sf::Texture& texture)
    {
        m_texture = &texture;
        m_needUpdate = true;
    }

    const sf::Color& BitmapNumber::GetColor() const
    {
        return m_vertices[0].color;
    }

    void BitmapNumber::SetColor(const sf::Color& color)
    {
        for (unsigned int i = 0; i < m_vertices.getVertexCount(); i++)
            m_vertices[i].color = color;
    }

    float BitmapNumber::GetKerning() const
    {
        return m_kerning;
    }

    void BitmapNumber::SetKerning(const float value)
    {
        m_kerning = value;
        m_needUpdate = true;
    }

    int BitmapNumber::GetDigitCount() const
    {
        return m_digitCount;
    }

    void BitmapNumber::SetDigitCount(const int count)
    {
        m_digitCount = count;
        m_needUpdate = true;
    }

    const sf::Time& BitmapNumber::GetAnimationDuration(const unsigned int digit) const
    {
        if (const auto it = m_durations.find(digit % 10); it != m_durations.end())
            return it->second;

        return sf::Time::Zero;
    }

    void BitmapNumber::SetAnimationDuration(const sf::Time& duration)
    {
        for (unsigned int digit = 0; digit < 10; digit++)
            m_durations[digit] = duration;
    }

    void BitmapNumber::SetAnimationDuration(const unsigned int digit, const sf::Time& duration)
    {
        m_durations[digit % 10] = duration;
    }

    void BitmapNumber::SetDigitFrames(const unsigned int digit, const std::vector<sf::IntRect>& texCoords)
    {
        m_texCoords[digit % 10] = texCoords;
        m_elapseds[digit % 10] = sf::Time::Zero;
        m_frames[digit % 10] = 0;

        m_needUpdate = true;
    }

    void BitmapNumber::SetDigitFrame(const unsigned int digit, sf::IntRect texCoords)
    {
        m_texCoords[digit % 10] = { texCoords };
        m_elapseds[digit % 10] = sf::Time::Zero;
        m_frames[digit % 10] = 0;

        m_needUpdate = true;
    }

    void BitmapNumber::SetDigitsSize(const sf::Vector2u size)
    {
        if (size == sf::Vector2u())
            return;

        m_texCoords.clear();
        unsigned int current = 0;
        for (unsigned int i = 0; i < 10; i++)
        {
            SetDigitFrame(i, sf::IntRect(sf::Vector2i(current, 0), sf::Vector2i(size.x, size.y)));
            current += size.x;
        }

        m_needUpdate = true;
    }

    unsigned long long BitmapNumber::GetValue() const
    {
        return m_value;
    }

    void BitmapNumber::SetValue(const unsigned long long value)
    {
        m_value = value;
        m_needUpdate = true;
    }

    BitmapNumber::Alignment BitmapNumber::GetAlignment() const
    {
        return m_alignment;
    }

    void BitmapNumber::SetAlignment(const Alignment alignment)
    {
        m_alignment = alignment;
        m_needUpdate = true;
    }

    BlendMode BitmapNumber::GetBlendMode() const
    {
        return m_blendMode;
    }

    void BitmapNumber::SetBlendMode(const BlendMode blendMode)
    {
        m_blendMode = blendMode;
    }

    Animation::AnimationState BitmapNumber::GetAnimationState() const
    {
        return m_state;
    }

    void BitmapNumber::SetAnimationCallback(const std::function<void(BitmapNumber&)> &animationCallback)
    {
        m_callback = animationCallback;
    }

    void BitmapNumber::Stop()
    {
        m_state = Animation::AnimationState::Stopped;
        if (m_callback)
            m_callback(*this);
    }

    void BitmapNumber::Reset()
    {
        for (auto& [digit, _] : m_frames)
        {
            m_frames[digit] = 0;
            m_elapseds[digit] = sf::Time::Zero;
            m_state = Animation::AnimationState::Initial;
            if (m_callback)
                m_callback(*this);
        }
    }

    void BitmapNumber::Update(const double delta)
    {
        if (m_state == Animation::AnimationState::Initial || m_state == Animation::AnimationState::Playing)
        {
            for (auto& [digit, elapsed] : m_durations)
            {
                if (m_texCoords[digit].size() <= 1)
                    continue;

                m_elapseds[digit] += sf::milliseconds(static_cast<int>(delta));
                if (const auto frameTime = sf::milliseconds(m_durations[digit].asMilliseconds() / static_cast<int>(m_texCoords[digit].size())); m_elapseds[digit] >= frameTime)
                {
                    if (m_state != Animation::AnimationState::Playing)
                    {
                        m_state = Animation::AnimationState::Playing;
                        if (m_callback)
                            m_callback(*this);
                    }

                    m_elapseds[digit] %= frameTime;
                    if (m_frames[digit] + 1 >= m_texCoords[digit].size())
                    {
                        m_state = Animation::AnimationState::Completed;
                        if (m_callback)
                            m_callback(*this);
                    }
                    else
                        m_frames[digit]++;

                    m_needUpdate = true;
                }
            }
        }

        if (m_needUpdate)
            Invalidate();

        Control::Update(delta);
    }

    RenderStates BitmapNumber::Render(RenderSurface& surface, RenderStates states) const
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

    void BitmapNumber::Invalidate()
    {
        const auto color = GetColor();
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

        auto digits = std::stack<unsigned int>();
        for (unsigned int d = 0; d < digitCount; d++)
        {
            if (d > 0)
                m_width += m_kerning;

            const bool isLeading = leadingCount > 0 && d > digitCount - leadingCount;
            if (isLeading)
                digit = 0;
            else
                digit = value % 10;

            if (!isLeading)
                value /= 10;

            auto texCoords = m_texCoords[digit];
            m_width += texCoords[m_frames[digit]].size.x;

            digits.push(digit);
        }

        auto position = sf::Vector2f();
        const auto size = digits.size();
        auto texCoords = sf::IntRect();
        for (unsigned int i = 0; i < size; i++)
        {
            digit = digits.top();
            digits.pop();

            if (i > 0)
                position += sf::Vector2f(texCoords.size.x + m_kerning, 0);

            texCoords = m_texCoords[digit][m_frames[digit]];
            const float x = position.x;
            const float y = position.y;
            const float w = position.x + texCoords.size.x;
            const float h = position.y + texCoords.size.y;

            const float left   = texCoords.position.x;
            const float top    = texCoords.position.y;
            const float right  = texCoords.position.x + texCoords.size.x;
            const float bottom = texCoords.position.y  + texCoords.size.y;

            const int index = i * 6;
            m_vertices[index + 0] = {sf::Vector2f(x, y), color, sf::Vector2f(left , top)};
            m_vertices[index + 1] = {sf::Vector2f(w, y), color, sf::Vector2f(right , top)};
            m_vertices[index + 2] = {sf::Vector2f(x, h), color, sf::Vector2f(left , bottom)};
            m_vertices[index + 3] = {sf::Vector2f(x, h), color, sf::Vector2f(left , bottom)};
            m_vertices[index + 4] = {sf::Vector2f(w, y), color, sf::Vector2f(right , top)};
            m_vertices[index + 5] = {sf::Vector2f(w, h), color, sf::Vector2f(right , bottom)};

            m_height = m_height < texCoords.size.y ? texCoords.size.y : m_height;
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
