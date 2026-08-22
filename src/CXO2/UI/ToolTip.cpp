#include <CXO2/UI/ToolTip.hpp>

#include <cmath>

namespace Cx
{
    ToolTip::ToolTip() :
        Gx::Text(),
        m_rectangle(),
        m_padding(1.f, 1.f),
        m_fillColor(sf::Color::White),
        m_outlineColor(sf::Color::Black),
        m_outlineThickness(1.f),
        m_duration(),
        m_elapsed(),
        m_delay(),
        m_delayElapsed(),
        m_pending(false)
    {
    }

    ToolTip::ToolTip(const Gx::Font& font, const sf::String& string, const unsigned int characterSize) :
        Gx::Text(font, string, characterSize),
        m_rectangle(),
        m_padding(characterSize * (10.f / 100.f), characterSize * (10.f / 100.f)),
        m_fillColor(sf::Color::White),
        m_outlineColor(sf::Color::Black),
        m_outlineThickness(1.f),
        m_duration(),
        m_elapsed(),
        m_delay(),
        m_delayElapsed(),
        m_pending(false)
    {
    }

    sf::FloatRect ToolTip::GetLocalBounds() const
    {
        return m_rectangle.GetLocalBounds();
    }

    void ToolTip::Show(const Control& parent)
    {
        auto position  = sf::Vector2f();
        auto alignment = Alignment::Left;
        auto bounds    = parent.GetLocalBounds();

        position  = sf::Vector2f(bounds.size.x, bounds.size.y) / 2.f;
        position  = sf::Vector2f(static_cast<int>(position.x), static_cast<int>(position.y));
        alignment = Alignment::Center;

        Show(position, alignment);
    }

    void ToolTip::Show(const sf::Vector2f position, const Alignment alignment)
    {
        if (alignment == Alignment::Center || alignment == Alignment::Right)
        {
            Invalidate();
            if (alignment == Alignment::Center)
                SetLineAlignment(LineAlignment::Center);
            else
                SetLineAlignment(LineAlignment::Right);
        }
        else
            SetLineAlignment(LineAlignment::Left);

        SetPosition(position);
        Show();
    }

    void ToolTip::Show()
    {
        m_delayElapsed = sf::Time::Zero;
        m_pending      = m_delay > sf::Time::Zero;
        m_elapsed      = m_pending ? m_duration : sf::Time::Zero;

        Invalidate();
    }

    void ToolTip::Hide()
    {
        m_delayElapsed = sf::Time::Zero;
        m_pending      = false;
        m_elapsed      = m_duration;
    }

    const sf::Time& ToolTip::GetDuration() const
    {
        return m_duration;
    }

    void ToolTip::SetDuration(const sf::Time& duration)
    {
        if (m_duration != duration)
        {
            const bool visible = m_elapsed < m_duration;
            m_duration = duration;
            m_elapsed = visible ? sf::Time::Zero : duration;
        }
    }

    const sf::Time& ToolTip::GetDelay() const
    {
        return m_delay;
    }

    void ToolTip::SetDelay(const sf::Time& delay)
    {
        m_delay = delay;
    }

    sf::Vector2f ToolTip::GetPadding() const
    {
        return m_padding;
    }

    void ToolTip::SetPadding(const sf::Vector2f& padding)
    {
        if (m_padding != padding)
        {
            m_padding = padding;
            Invalidate();
        }
    }

    const sf::Color& ToolTip::GetContainerColor() const
    {
        return m_fillColor;
    }

    void ToolTip::SetContainerColor(const sf::Color& fillColor)
    {
        if (m_fillColor != fillColor)
        {
            m_fillColor = fillColor;
            Invalidate();
        }
    }

    const sf::Color& ToolTip::GetContainerOutlineColor() const
    {
        return m_outlineColor;
    }

    void ToolTip::SetContainerOutlineColor(const sf::Color& outlineColor)
    {
        if (m_outlineColor != outlineColor)
        {
            m_outlineColor = outlineColor;
            Invalidate();
        }
    }

    float ToolTip::GetContainerOutlineThickness() const
    {
        return m_outlineThickness;
    }

    void ToolTip::SetContainerOutlineThickness(const float outlineThickness)
    {
        if (m_outlineThickness != outlineThickness)
        {
            m_outlineThickness = outlineThickness;
            Invalidate();
        }
    }

    Gx::RenderStates ToolTip::Render(Gx::RenderSurface& surface, const Gx::RenderStates states) const
    {
        if (IsVisible() && !GetString().isEmpty())
        {
            auto tempStates = states;
            tempStates.transform *= GetTransform();
            surface.Render(m_rectangle, tempStates);
        }

        return Label::Render(surface, states);
    }

    void ToolTip::Update(const sf::Time& delta)
    {
        Label::Update(delta);

        if (m_pending)
        {
            m_delayElapsed += delta;
            if (m_delayElapsed >= m_delay)
            {
                m_delayElapsed = sf::Time::Zero;
                m_pending      = false;
                m_elapsed      = sf::Time::Zero;

                Invalidate();
            }
        }
        else if (m_elapsed < m_duration)
            m_elapsed += delta;

        if (IsVisible())
            Invalidate();
    }

    void ToolTip::Invalidate()
    {
        Label::Invalidate();

        const auto bounds = Gx::Text::GetLocalBounds();
        const auto size   = sf::Vector2f(bounds.size.x, bounds.size.y) + (m_padding * 2.f);

        if (GetLineAlignment() != LineAlignment::Center)
            m_rectangle.SetPosition(sf::Vector2f(0, std::ceil(m_padding.y * 1.5f)));
        else
            m_rectangle.SetPosition(sf::Vector2f(-1.f * (std::ceil(bounds.size.x / 2.f) + m_padding.x), std::ceil(m_padding.y * 1.5f)));

        m_rectangle.SetSize({ std::ceil(size.x), std::ceil(size.y) });
        m_rectangle.SetColor(m_fillColor);
        m_rectangle.SetOutlineColor(m_outlineColor);
        m_rectangle.SetOutlineThickness(m_outlineThickness);

        SetVisible(m_elapsed < m_duration);
    }
}
