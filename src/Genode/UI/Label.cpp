#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Font.hpp>

#include <cmath>

namespace Gx
{
    void Label::AddFallbackFont(const Gx::Font& font) const
    {
        if (m_defaultFont == nullptr)
            m_defaultFont = GetFont();

        m_fallbackFonts.insert(&font);
    }

    sf::FloatRect Label::GetLocalBounds() const
    {
        if (m_bounds != sf::FloatRect{})
            return m_bounds;

        return Text::GetLocalBounds();
    }

    void Label::SetLocalBounds(const sf::FloatRect& bounds)
    {
        m_bounds = bounds;
        Invalidate();
    }

    void Label::Update(const double delta)
    {
        Control::Update(delta);

        Invalidate();
    }

    RenderStates Label::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        return Text::Render(surface, states);
    }

    void Label::OnFontChanged(const Gx::Font& font) const
    {
        Text::OnFontChanged(font);
        m_defaultFont = &font;
    }

    Label::Alignment Label::GetAlignment() const
    {
        return m_alignment;
    }

    void Label::SetAlignment(const Alignment alignment)
    {
        if (m_alignment == alignment)
            return;

        m_alignment = alignment;
        m_alignmentUpdated = false;
        Invalidate();
    }

    void Label::OnGeometryUpdating() const
    {
        Text::OnGeometryUpdating();

        if (m_fallbackFonts.empty())
            return;

        if (m_defaultFont == nullptr)
            m_defaultFont = GetFont();

        auto candidates = std::unordered_set<const Font*>{};
        for (const auto& font : m_fallbackFonts)
            candidates.insert(font);

        bool useDefault = m_defaultFont != nullptr;
        for (const char32_t codepoint : GetString())
        {
            useDefault = useDefault && m_defaultFont->HasGlyph(codepoint);
            for (auto& font : m_fallbackFonts)
            {
                if (candidates.find(font) == candidates.end())
                    continue;

                if (!font->HasGlyph(codepoint))
                    candidates.erase(font);
            }

            if (candidates.empty())
                return;
        }

        if (useDefault)
        {
            const_cast<Label*>(this)->SetFont(*m_defaultFont);
            return;
        }

        for (auto& candidate : candidates)
        {
            if (candidate)
            {
                const_cast<Label*>(this)->SetFont(*candidate);
                return;
            }
        }
    }

    void Label::OnGeometryUpdated() const
    {
        Text::OnGeometryUpdated();
        m_alignmentUpdated = false;
    }

    void Label::Invalidate()
    {
        EnsureGeometryUpdate();

        if (m_bounds != sf::FloatRect{})
        {
            auto string = GetString();
            std::size_t checkpoint = 0;
            for (std::size_t c = 0; c < string.getSize(); c++)
            {
                if (string[c] == '\n')
                    continue;

                if (string[c] == ' ')
                {
                    checkpoint = c;
                    continue;
                }

                const auto position = FindCharacterPosition(c);
                if (m_bounds.position.x + position.x > m_bounds.position.x + m_bounds.size.x)
                {
                    if (checkpoint == 0 && c > 0)
                        checkpoint = c - 1;

                    if (string[checkpoint] != '\n')
                    {
                        string.replace(checkpoint, 1, "\n");
                        SetString(string);

                        c = 0;
                    }
                    else
                    {
                        checkpoint = 0;
                    }
                }
            }

            EnsureGeometryUpdate();
        }

        if (m_alignmentUpdated)
            return;

        const auto bounds = GetLocalBounds();
        const auto origin = GetOrigin();

        if (m_alignment == Alignment::Left)
            SetOrigin(bounds.position.x, origin.y);
        else if (m_alignment == Alignment::Center)
            SetOrigin(std::floor(bounds.position.x + (bounds.size.x / 2.f)), origin.y);
        else if (m_alignment == Alignment::Right)
            SetOrigin((bounds.position.x + bounds.size.x), origin.y);

        m_alignmentUpdated = true;
    }
}
