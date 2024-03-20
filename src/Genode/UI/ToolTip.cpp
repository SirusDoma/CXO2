#include <Genode/UI/ToolTip.hpp>

Gx::ToolTip::ToolTip() :
    Text(),
    m_rectangle(),
    m_padding(1.f, 1.f),
    m_fillColor(sf::Color::White),
    m_outlineColor(sf::Color::Black),
    m_outlineThickness(1.f),
    m_elapsed(),
    m_duration()
{
}

Gx::ToolTip::ToolTip(const sf::String &string, const sf::Font &font, unsigned int characterSize) :
    Text(string, font, characterSize),
    m_rectangle(),
    m_padding(characterSize * (10.f / 100.f), characterSize * (10.f / 100.f)),
    m_fillColor(sf::Color::White),
    m_outlineColor(sf::Color::Black),
    m_outlineThickness(1.f),
    m_elapsed(),
    m_duration()
{
}

sf::FloatRect Gx::ToolTip::GetLocalBounds() const
{
    return m_rectangle.GetLocalBounds();
}

void Gx::ToolTip::Show(Gx::Control *parent)
{
    auto position  = sf::Vector2f();
    auto alignment = Alignment::Left;
    if (parent)
    {
        position  = sf::Vector2f(parent->GetLocalBounds().width, parent->GetLocalBounds().height) / 2.f;
        position  = sf::Vector2f(static_cast<int>(position.x), static_cast<int>(position.y));
        alignment = Alignment::Center;
    }

    Show(position, alignment);
}

void Gx::ToolTip::Show(sf::Vector2f position, Alignment alignment)
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

void Gx::ToolTip::Show()
{
    m_elapsed = sf::Time::Zero;
    Invalidate();
}

void Gx::ToolTip::Hide()
{
    m_elapsed = m_duration;
}

const sf::Time &Gx::ToolTip::GetDuration() const
{
    return m_duration;
}

void Gx::ToolTip::SetDuration(const sf::Time &duration)
{
    if (m_duration != duration)
    {
        bool visible = m_elapsed < m_duration;
        m_duration   = duration;
        m_elapsed    = visible ? sf::Time::Zero : duration;
    }
}

sf::Vector2f Gx::ToolTip::GetPadding() const
{
    return m_padding;
}

void Gx::ToolTip::SetPadding(const sf::Vector2f &padding)
{
    if (m_padding != padding)
    {
        m_padding = padding;
        Invalidate();
    }
}

const sf::Color &Gx::ToolTip::GetContainerFillColor() const
{
    return m_fillColor;
}

void Gx::ToolTip::SetContainerFillColor(const sf::Color &fillColor)
{
    if (m_fillColor != fillColor)
    {
        m_fillColor = fillColor;
        Invalidate();
    }
}

const sf::Color &Gx::ToolTip::GetContainerOutlineColor() const
{
    return m_outlineColor;
}

void Gx::ToolTip::SetContainerOutlineColor(const sf::Color &outlineColor)
{
    if (m_outlineColor != outlineColor)
    {
        m_outlineColor = outlineColor;
        Invalidate();
    }
}

float Gx::ToolTip::GetContainerOutlineThickness() const
{
    return m_outlineThickness;
}

void Gx::ToolTip::SetContainerOutlineThickness(float outlineThickness)
{
    if (m_outlineThickness != outlineThickness)
    {
        m_outlineThickness = outlineThickness;
        Invalidate();
    }
}

sf::RenderStates Gx::ToolTip::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (IsVislble() && !GetString().isEmpty())
    {
        auto tempStates = states;
        tempStates.transform *= GetTransform();
        target.draw(m_rectangle, tempStates);
    }

    return Label::Render(target, states);
}

void Gx::ToolTip::Update(double delta)
{
    Label::Update(delta);

    if (m_elapsed < m_duration)
        m_elapsed += sf::milliseconds(delta);

    if (IsVislble())
        Invalidate();
}

void Gx::ToolTip::Invalidate()
{
    Label::Invalidate();

    auto bounds = Text::GetLocalBounds();
    m_rectangle.SetSize(sf::Vector2f(bounds.width, bounds.height) + (m_padding * 2.f));
    m_rectangle.SetPosition(sf::Vector2f(0, m_padding.y * 1.5f));
    m_rectangle.SetFillColor(m_fillColor);
    m_rectangle.SetOutlineColor(m_outlineColor);
    m_rectangle.SetOutlineThickness(m_outlineThickness);

    SetVisible(m_elapsed < m_duration);
}
