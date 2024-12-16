#include <Genode/UI/Gauge.hpp>

namespace Gx
{
    Gauge::Gauge() :
        Control(),
        m_vertices(sf::PrimitiveType::TriangleStrip, 4),
        m_texture(),
        m_texCoords(),
        m_orientation(Orientation::Horizontal),
        m_value(0),
        m_maximum(100.0f),
        m_flicker(false),
        m_flickerActivate(false),
        m_slanted(false),
        m_animationState(Animation::AnimationState::Initial),
        m_currentFrame(0)
    {
    }

    Gauge::Gauge(const sf::Texture& texture) :
        Gauge(texture, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)))
    {
    }

    Gauge::Gauge(const sf::Texture& texture, const sf::IntRect& texCoords) :
        Gauge()
    {
        SetTexture(texture);
        SetTexCoords(texCoords);
    }

    sf::FloatRect Gauge::GetLocalBounds() const
    {
        return {sf::Vector2f(0, 0), sf::Vector2f(m_texCoords.size.x, m_texCoords.size.y)};
    }

    bool Gauge::IsFlickering() const
    {
        return m_flicker;
    }

    void Gauge::SetFlickering(const bool flicker)
    {
        if (m_flicker != flicker)
        {
            m_flicker = flicker;
            Invalidate();
        }
    }

    bool Gauge::IsSlanted() const
    {
        return m_slanted;
    }

    void Gauge::SetSlanted(const bool slanted)
    {
        m_slanted = slanted;
    }

    Animation::AnimationState Gauge::GetAnimationState() const
    {
        return m_animationState;
    }

    void Gauge::AddAnimationFrame(const Animation::Frame& frame)
    {
        m_frames.push_back(frame);
        if (m_frames.size() == 1)
        {
            m_currentFrame = 0;
            Invalidate();
        }
    }

    unsigned int Gauge::GetAnimationFrameCount() const
    {
        return m_frames.size();
    }

    const sf::Time& Gauge::GetAnimationDuration() const
    {
        return m_animationDuration;
    }

    void Gauge::SetAnimationDuration(const sf::Time& duration)
    {
        m_animationDuration = duration;
    }

    const sf::Texture* Gauge::GetTexture() const
    {
        return m_texture;
    }

    void Gauge::SetTexture(const sf::Texture& texture)
    {
        m_texture = &texture;
    }

    const sf::IntRect& Gauge::GetTexCoords() const
    {
        return m_texCoords;
    }

    void Gauge::SetTexCoords(const sf::IntRect& texCoords)
    {
        if (texCoords != m_texCoords)
        {
            m_texCoords = texCoords;
            Invalidate();
        }
    }

    const sf::Color& Gauge::GetColor() const
    {
        return m_vertices[0].color;
    }

    void Gauge::SetColor(const sf::Color& color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }

    Gauge::Orientation Gauge::GetOrientation() const
    {
        return m_orientation;
    }

    void Gauge::SetOrientation(const Orientation& orientation)
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

        if (!IsEnabled() || m_frames.empty())
            return;

        m_animationState    = Animation::AnimationState::Playing;
        m_animationElapsed += sf::milliseconds(static_cast<int>(delta));
        if (const auto frameTime = sf::milliseconds(m_animationDuration.asMilliseconds() / static_cast<int>(m_frames.size())); m_animationElapsed >= frameTime)
        {
            m_animationElapsed %= frameTime;
            m_currentFrame++;
            if (m_currentFrame >= m_frames.size())
                m_currentFrame = 0;

            m_flickerActivate = m_flicker && !m_flickerActivate;
            Invalidate();
        }

    }

    RenderStates Gauge::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_texture)
        {
            states.transform *= GetTransform();
            states.texture    = m_texture;
            surface.Render(m_vertices, states);
        }

        return Control::Render(surface, states);
    }

    void Gauge::Invalidate()
    {
        if (m_currentFrame < m_frames.size())
        {
            const auto& frame = m_frames[m_currentFrame];
            m_texCoords = frame.TexCoords;

            if (frame.Origin.has_value())
                SetOrigin(frame.Origin.value());

            if (frame.Position.has_value())
                SetPosition(frame.Position.value());

            if (frame.Rotation.has_value())
                SetRotation(frame.Rotation.value());

            if (frame.Scale.has_value())
                SetScale(frame.Scale.value());
        }

        const auto color  = GetColor();
        const auto bounds = GetLocalBounds();
        const int x   = bounds.position.x;
        int y         = bounds.position.y;
        float w       = bounds.size.x;
        const float h = bounds.size.y;

        const auto left   = static_cast<float>(m_texCoords.position.x);
        float right        = left + w;
        auto top          = static_cast<float>(m_texCoords.position.y);
        const float bottom = top + h;

        if (m_value < m_maximum)
        {
            if (m_orientation == Orientation::Horizontal)
            {
                const float progress = bounds.size.x * (m_value / m_maximum);
                w     = progress;
                right = left + w;

                if (m_flickerActivate && m_value != m_maximum && m_value != 0 && progress > 0)
                {
                    w -= 5;
                    right -= 5;
                }
            }
            else
            {
                const float progress = bounds.size.y * (1.f - (m_value / m_maximum));
                y   = progress;
                top = progress;

                if (m_flickerActivate && m_value != m_maximum && m_value != 0 && progress > 0)
                {
                    y -= 5;
                    top -= 5;
                }
            }
        }

        m_vertices[0] = {sf::Vector2f(x, y + (m_slanted && m_value != m_maximum && m_value != 0 && y + 10 < y + bottom ? 10 : 0)), color, sf::Vector2f(left, top)};
        m_vertices[1] = {sf::Vector2f(x, h), color, sf::Vector2f(left, bottom)};
        m_vertices[2] = {sf::Vector2f(w, y), color, sf::Vector2f(right, top)};
        m_vertices[3] = {sf::Vector2f(w, h), color, sf::Vector2f(right, bottom)};
    }
}
