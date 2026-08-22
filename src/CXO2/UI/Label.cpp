#include <CXO2/UI/Label.hpp>
#include <Genode/Graphics/Font.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace
{
    struct LineMetrics
    {
        float Baseline;
        float MinX;
        float MaxX;
        std::size_t FirstCluster;
    };

    bool IsBlank(const char32_t ch)
    {
        return ch == U' ' || ch == U'\n' || ch == U'\t';
    }

    void TrimTail(sf::String& string)
    {
        while (!string.isEmpty() && IsBlank(string[string.getSize() - 1]))
            string.erase(string.getSize() - 1);
    }

    std::vector<LineMetrics> MeasureLines(const Gx::Text& text)
    {
        auto lines = std::vector<LineMetrics>();

        const auto& string = text.GetString();
        for (const auto& glyph : text.GetShapedGlyphs())
        {
            const auto cluster = static_cast<std::size_t>(glyph.cluster);
            if (cluster >= string.getSize() || string[cluster] == U'\n')
                continue;

            const float left  = glyph.position.x;
            const float right = left + glyph.glyph.advance + (glyph.glyph.advance > 0.f ? text.GetLetterSpacing() : 0.f);

            const auto line = std::find_if(lines.begin(), lines.end(),
                [&] (const auto& entry) { return entry.Baseline == glyph.baseline; });

            if (line == lines.end())
            {
                lines.push_back(LineMetrics{glyph.baseline, left, right, cluster});
                continue;
            }

            line->MinX         = std::min(line->MinX, left);
            line->MaxX         = std::max(line->MaxX, right);
            line->FirstCluster = std::min(line->FirstCluster, cluster);
        }

        std::sort(lines.begin(), lines.end(),
            [] (const auto& left, const auto& right) { return left.Baseline < right.Baseline; });

        return lines;
    }

    void ClipQuadArray(sf::VertexArray& vertices, const sf::FloatRect& rect)
    {
        const float left   = rect.position.x;
        const float top    = rect.position.y;
        const float right  = left + rect.size.x;
        const float bottom = top + rect.size.y;

        std::size_t write       = 0;
        const std::size_t count = vertices.getVertexCount();
        for (std::size_t i = 0; i + 6 <= count; i += 6)
        {
            float minX = vertices[i].position.x,  maxX = minX;
            float minY = vertices[i].position.y,  maxY = minY;
            float minU = vertices[i].texCoords.x, maxU = minU;
            float minV = vertices[i].texCoords.y, maxV = minV;

            for (std::size_t v = 1; v < 6; v++)
            {
                const auto& vertex = vertices[i + v];

                minX = std::min(minX, vertex.position.x);
                maxX = std::max(maxX, vertex.position.x);
                minY = std::min(minY, vertex.position.y);
                maxY = std::max(maxY, vertex.position.y);
                minU = std::min(minU, vertex.texCoords.x);
                maxU = std::max(maxU, vertex.texCoords.x);
                minV = std::min(minV, vertex.texCoords.y);
                maxV = std::max(maxV, vertex.texCoords.y);
            }

            if (minX >= right || maxX <= left || minY >= bottom || maxY <= top)
                continue;

            for (std::size_t v = 0; v < 6; v++)
            {
                auto vertex = vertices[i + v];
                if (maxX > minX)
                {
                    const float x      = std::clamp(vertex.position.x, left, right);
                    vertex.texCoords.x = minU + (maxU - minU) * ((x - minX) / (maxX - minX));
                    vertex.position.x  = x;
                }

                if (maxY > minY)
                {
                    const float y      = std::clamp(vertex.position.y, top, bottom);
                    vertex.texCoords.y = minV + (maxV - minV) * ((y - minY) / (maxY - minY));
                    vertex.position.y  = y;
                }

                vertices[write + v] = vertex;
            }

            write += 6;
        }

        if (write < count)
            vertices.resize(write);
    }
}

namespace Cx
{
    void Label::AddFallbackFont(const Gx::Font& font) const
    {
        if (m_defaultFont == nullptr)
            m_defaultFont = GetFont();

        m_fallbackFonts.insert(&font);
    }

    void Label::SetString(const sf::String& string)
    {
        if (m_hasSource && m_sourceString == string)
            return;

        m_sourceString = string;
        m_hasSource    = true;

        Gx::Text::SetString(string);
    }

    Label::VerticalAlignment Label::GetVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    void Label::SetVerticalAlignment(const VerticalAlignment alignment)
    {
        if (m_verticalAlignment == alignment)
            return;

        m_verticalAlignment = alignment;
        m_layoutNeeded      = true;

        Invalidate();
    }

    const sf::String& Label::GetEllipsis() const
    {
        return m_ellipsis;
    }

    void Label::SetEllipsis(const sf::String& ellipsis)
    {
        if (m_ellipsis == ellipsis)
            return;

        m_ellipsis     = ellipsis;
        m_layoutNeeded = true;

        Invalidate();
    }

    sf::FloatRect Label::GetLocalBounds() const
    {
        if (m_bounds != sf::FloatRect{})
            return m_bounds;

        return Gx::Text::GetLocalBounds();
    }

    void Label::SetLocalBounds(const sf::FloatRect& bounds)
    {
        if (m_bounds == bounds)
            return;

        m_bounds       = bounds;
        m_layoutNeeded = true;

        Invalidate();
    }

    void Label::ClipQuads(const sf::FloatRect& rect) const
    {
        EnsureGeometryUpdate();

        const auto offset  = GetOutlineOffset();
        const auto padding = sf::Vector2f(
            1.f + GetOutlineThickness() + std::abs(offset.x),
            1.f + GetOutlineThickness() + std::abs(offset.y)
        );

        const auto clip = sf::FloatRect(rect.position - padding, rect.size + padding * 2.f);
        ClipQuadArray(GetVertexData(), clip);
        ClipQuadArray(GetOutlineVertexData(), clip);
    }

    void Label::Update(const sf::Time& delta)
    {
        Control::Update(delta);

        Invalidate();
    }

    Gx::RenderStates Label::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        EnsureLayout();

        if (m_bounds != sf::FloatRect{})
            states.transform.translate(m_alignOffset);

        return Gx::Text::Render(surface, states);
    }

    void Label::OnFontChanged(const Gx::Font& font) const
    {
        Gx::Text::OnFontChanged(font);
        m_defaultFont = &font;
    }

    void Label::OnGeometryUpdating() const
    {
        Gx::Text::OnGeometryUpdating();

        if (m_fallbackFonts.empty())
            return;

        if (m_defaultFont == nullptr)
            m_defaultFont = GetFont();

        auto candidates = std::unordered_set<const Gx::Font*>{};
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
        Gx::Text::OnGeometryUpdated();
        m_layoutNeeded = true;
    }

    void Label::Invalidate()
    {
        EnsureLayout();
    }

    void Label::EnsureLayout() const
    {
        EnsureGeometryUpdate();

        if (!m_layoutNeeded)
            return;

        m_layoutNeeded = false;
        Layout();
        m_layoutNeeded = false;
    }

    void Label::SetDisplayString(const sf::String& string) const
    {
        const_cast<Label*>(this)->Gx::Text::SetString(string);
    }

    void Label::Layout() const
    {
        if (!GetFont())
            return;

        if (!m_hasSource)
        {
            m_sourceString = GetString();
            m_hasSource    = true;
        }

        m_alignOffset = sf::Vector2f();
        if (GetString() != m_sourceString)
            SetDisplayString(m_sourceString);

        if (m_bounds == sf::FloatRect{})
            return;

        constexpr auto npos = std::numeric_limits<std::size_t>::max();
        const float width   = m_bounds.size.x;

        const float characterHeight = static_cast<float>(GetCharacterHeight());
        const float ascent          = GetFont()->GetAscent(GetCharacterWidth(), GetCharacterHeight());
        const float descent         = GetFont()->GetDescent(GetCharacterWidth(), GetCharacterHeight());
        const float lineAdvance     = GetFont()->GetLineSpacing(GetCharacterWidth(), GetCharacterHeight()) + GetLineSpacing();

        auto maxLines = static_cast<std::size_t>(lineAdvance > 0.f ? m_bounds.size.y / lineAdvance : 1.f);
        maxLines      = std::max<std::size_t>(maxLines, 1);

        auto display = m_sourceString;
        bool wrapped = true;
        while (wrapped)
        {
            wrapped = false;

            const auto lines = MeasureLines(*this);
            if (lines.size() >= maxLines)
                break;

            for (const auto& line : lines)
            {
                if (line.MaxX - line.MinX <= width)
                    continue;

                bool overflow          = false;
                std::size_t checkpoint = npos;
                for (const auto& glyph : GetShapedGlyphs())
                {
                    if (glyph.baseline != line.Baseline)
                        continue;

                    const auto cluster = static_cast<std::size_t>(glyph.cluster);
                    if (cluster >= display.getSize() || display[cluster] == U'\n')
                        continue;

                    if (display[cluster] == U' ')
                    {
                        checkpoint = cluster;
                        continue;
                    }

                    const float right = glyph.position.x + glyph.glyph.advance + (glyph.glyph.advance > 0.f ? GetLetterSpacing() : 0.f);
                    if (right - line.MinX > width)
                    {
                        overflow = true;
                        break;
                    }
                }

                if (!overflow || checkpoint == npos)
                    continue;

                display.replace(checkpoint, 1, "\n");
                SetDisplayString(display);

                wrapped = true;
                break;
            }
        }

        if (!m_ellipsis.isEmpty())
        {
            auto lines = MeasureLines(*this);
            bool cut   = false;

            if (lines.size() > maxLines)
            {
                display = display.substring(0, lines[maxLines].FirstCluster);
                TrimTail(display);
                SetDisplayString(display);

                lines = MeasureLines(*this);
                cut   = true;
            }

            if (!lines.empty() && (cut || lines.back().MaxX - lines.back().MinX > width))
            {
                while (true)
                {
                    SetDisplayString(display + m_ellipsis);

                    lines = MeasureLines(*this);
                    if (lines.empty() || display.isEmpty())
                        break;

                    const auto& last = lines.back();
                    if (last.MaxX - last.MinX <= width)
                        break;

                    display.erase(display.getSize() - 1);
                    TrimTail(display);
                }
            }
        }

        const auto lineCount   = std::max<std::size_t>(MeasureLines(*this).size(), 1);
        const float textHeight = (ascent - descent) + static_cast<float>(lineCount - 1) * lineAdvance;

        float x = m_bounds.position.x;
        if (GetLineAlignment() == LineAlignment::Center)
            x += width / 2.f;
        else if (GetLineAlignment() == LineAlignment::Right)
            x += width;

        float y          = m_bounds.position.y + (ascent - characterHeight);
        const float room = std::max(0.f, m_bounds.size.y - textHeight);
        if (m_verticalAlignment == VerticalAlignment::Center)
            y += room / 2.f;
        else if (m_verticalAlignment == VerticalAlignment::Bottom)
            y += room;

        m_alignOffset = sf::Vector2f(std::floor(x), std::floor(y));

        ClipQuads(sf::FloatRect(m_bounds.position - m_alignOffset, m_bounds.size));
    }
}
