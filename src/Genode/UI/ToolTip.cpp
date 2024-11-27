#include <Genode/UI/ToolTip.hpp>

namespace Gx
{
    ToolTip::ToolTip() :
        Text(),
        m_rectangle(),
        m_padding(1.f, 1.f),
        m_fillColor(sf::Color::White),
        m_outlineColor(sf::Color::Black),
        m_outlineThickness(1.f),
        m_duration(),
        m_elapsed()
    {
    }

    ToolTip::ToolTip(const Font& font, const sf::String& string, const unsigned int characterSize) :
        Text(font, string, characterSize),
        m_rectangle(),
        m_padding(characterSize * (10.f / 100.f), characterSize * (10.f / 100.f)),
        m_fillColor(sf::Color::White),
        m_outlineColor(sf::Color::Black),
        m_outlineThickness(1.f),
        m_duration(),
        m_elapsed()
    {
    }

    sf::FloatRect ToolTip::GetLocalBounds() const
    {
        return m_rectangle.GetLocalBounds();
    }

    void ToolTip::Show(Control* parent)
    {
        auto position = sf::Vector2f();
        auto alignment = Alignment::Left;
        if (parent)
        {
            position = sf::Vector2f(parent->GetLocalBounds().size.x, parent->GetLocalBounds().size.y) / 2.f;
            position = sf::Vector2f(static_cast<int>(position.x), static_cast<int>(position.y));
            alignment = Alignment::Center;
        }

        Show(position, alignment);
    }

    void ToolTip::Show(sf::Vector2f position, Alignment alignment)
    {
        if (alignment == Alignment::Center || alignment == Alignment::Right)
        {
            Invalidate();
            if (alignment == Alignment::Center)
                SetOrigin(m_rectangle.GetSize().x / 2.f, m_rectangle.GetSize().y / 2.f);
            else
                SetOrigin(m_rectangle.GetSize().x, m_rectangle.GetSize().y);
        }
        else
            SetOrigin(sf::Vector2f());

        SetPosition(position);
        Show();
    }

    void ToolTip::Show()
    {
        m_elapsed = sf::Time::Zero;
        Invalidate();
    }

    void ToolTip::Hide()
    {
        m_elapsed = m_duration;
    }

    const sf::Time& ToolTip::GetDuration() const
    {
        return m_duration;
    }

    void ToolTip::SetDuration(const sf::Time& duration)
    {
        if (m_duration != duration)
        {
            bool visible = m_elapsed < m_duration;
            m_duration = duration;
            m_elapsed = visible ? sf::Time::Zero : duration;
        }
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

    void ToolTip::SetContainerOutlineThickness(float outlineThickness)
    {
        if (m_outlineThickness != outlineThickness)
        {
            m_outlineThickness = outlineThickness;
            Invalidate();
        }
    }

    RenderStates ToolTip::Render(RenderSurface& surface, RenderStates states) const
    {
        if (IsVisible() && !GetString().isEmpty())
        {
            auto tempStates = states;
            tempStates.transform *= GetTransform();
            surface.Render(m_rectangle, tempStates);
        }

        return Label::Render(surface, states);
    }

    void ToolTip::Update(const double delta)
    {
        Label::Update(delta);

        if (m_elapsed < m_duration)
            m_elapsed += sf::milliseconds(delta);

        if (IsVisible())
            Invalidate();
    }

    void ToolTip::Invalidate()
    {
        Label::Invalidate();

        auto bounds = Text::GetLocalBounds();
        m_rectangle.SetSize(sf::Vector2f(bounds.size.x, bounds.size.y) + (m_padding * 2.f));
        m_rectangle.SetPosition(sf::Vector2f(0, m_padding.y * 1.5f));
        m_rectangle.SetColor(m_fillColor);
        m_rectangle.SetOutlineColor(m_outlineColor);
        m_rectangle.SetOutlineThickness(m_outlineThickness);

        SetVisible(m_elapsed < m_duration);
    }
}