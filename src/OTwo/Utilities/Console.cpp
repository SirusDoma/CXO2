#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <OTwo/Utilities/Console.hpp>

Console& Console::Instance()
{
    if (!m_instance)
        m_instance = std::make_unique<Console>();

    return *m_instance;
}

bool Console::IsEnabled() const
{
    return m_enabled;
}

void Console::SetEnabled(const bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Invalidate();
}

const Gx::Font* Console::GetFont() const
{
    return m_text.GetFont();
}

void Console::SetFont(const Gx::Font& font)
{
    if (m_text.GetFont() == &font)
        return;

    m_text.SetFont(font);
    Invalidate();
}

unsigned int Console::GetCharacterSize() const
{
    return m_text.GetCharacterSize();
}

void Console::SetCharacterSize(const unsigned int size)
{
    if (m_text.GetCharacterSize() == size)
        return;

    m_text.SetCharacterSize(size);
    Invalidate();
}

float Console::GetLineSpacing() const
{
    return m_text.GetLineSpacing();
}

void Console::SetLineSpacing(const float spacing)
{
    if (m_text.GetLineSpacing() == spacing)
        return;

    m_text.SetLetterSpacing(spacing);
    Invalidate();
}

unsigned int Console::GetMaximumLines() const
{
    return m_maxLines;
}

void Console::SetMaximumLines(const unsigned int max)
{
    if (m_maxLines == max)
        return;

    m_maxLines = max;
    Invalidate();
}

sf::FloatRect Console::GetBounds() const
{
    return m_bounds;
}

void Console::SetBounds(const sf::FloatRect& bounds)
{
    if (m_bounds == bounds)
        return;

    m_bounds = bounds;
    Invalidate();
}

void Console::Write(const std::string& message)
{
    m_logs.push_back(message);
    Invalidate();
}

Gx::RenderStates Console::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    if (!m_enabled || !m_text.GetFont())
        return states;

    auto backdrop = Gx::Rectangle(m_bounds.size);
    backdrop.SetColor(sf::Color(0, 0, 0, 127));

    states.transform *= GetTransform();
    surface.Render(backdrop, states);

    states.transform.translate({GetCharacterSize() / 4.f, GetCharacterSize() / 4.f});
    surface.Render(m_text, states);

    return states;
}

void Console::Invalidate()
{
    if (!m_text.GetFont())
        return;

    m_text.SetString(std::string());
    for (std::size_t i = std::max(static_cast<int>(m_logs.size() - m_maxLines), 0); i < m_logs.size(); i++)
    {
        if (i >= static_cast<int>(m_logs.size() + m_maxLines))
            break;

        m_text.SetString(m_text.GetString() + m_logs[i] + "\n");
    }
}
