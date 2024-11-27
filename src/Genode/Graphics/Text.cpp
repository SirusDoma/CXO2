////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2024 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Genode/Graphics/Text.hpp>
#include <Genode/Graphics/Font.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

namespace
{
    struct CacheID
    {
        typedef std::uint64_t sf::Texture::* type;
        friend type Get(CacheID);
    };

    template<typename Tag, typename Tag::type M>
    struct Private
    {
        friend typename Tag::type Get(Tag)
        {
            return M;
        }
    };

    template struct Private<CacheID, &sf::Texture::m_cacheId>;

    // Add an underline or strikethrough line to the vertex array
    void AddLine(sf::VertexArray& vertices,
        const float lineLength,
        const float lineTop,
        const sf::Color color,
        const float offset,
        const float thickness,
        const float outlineThickness = 0)
    {
        const float top    = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
        const float bottom = top + std::floor(thickness + 0.5f);

        vertices.append({{-outlineThickness, top - outlineThickness}, color, {1.0f, 1.0f}});
        vertices.append({{lineLength + outlineThickness, top - outlineThickness}, color, {1.0f, 1.0f}});
        vertices.append({{-outlineThickness, bottom + outlineThickness}, color, {1.0f, 1.0f}});
        vertices.append({{-outlineThickness, bottom + outlineThickness}, color, {1.0f, 1.0f}});
        vertices.append({{lineLength + outlineThickness, top - outlineThickness}, color, {1.0f, 1.0f}});
        vertices.append({{lineLength + outlineThickness, bottom + outlineThickness}, color, {1.0f, 1.0f}});
    }

    // Add a glyph quad to the vertex array
    void AddGlyphQuad(
        sf::VertexArray& vertices,
        const sf::Vector2f position,
        const sf::Color& color,
        const sf::Glyph& glyph,
        const float italicShear,
        const sf::Vector2f offset = sf::Vector2f(0.f, 0.f),
        const sf::Vector2f padding = sf::Vector2f(1.f, 1.f)
    )
    {
        const sf::Vector2f p1 = glyph.bounds.position - sf::Vector2f{1.f, 1.f};
        const sf::Vector2f p2 = glyph.bounds.position + glyph.bounds.size + padding + offset;

        const auto uv1 = sf::Vector2f(glyph.textureRect.position) - padding;
        const auto uv2 = sf::Vector2f(glyph.textureRect.position + glyph.textureRect.size) + padding;

        vertices.append({position + sf::Vector2f(p1.x - italicShear * p1.y, p1.y), color, {uv1.x, uv1.y}});
        vertices.append({position + sf::Vector2f(p2.x - italicShear * p1.y, p1.y), color, {uv2.x, uv1.y}});
        vertices.append({position + sf::Vector2f(p1.x - italicShear * p2.y, p2.y), color, {uv1.x, uv2.y}});
        vertices.append({position + sf::Vector2f(p1.x - italicShear * p2.y, p2.y), color, {uv1.x, uv2.y}});
        vertices.append({position + sf::Vector2f(p2.x - italicShear * p1.y, p1.y), color, {uv2.x, uv1.y}});
        vertices.append({position + sf::Vector2f(p2.x - italicShear * p2.y, p2.y), color, {uv2.x, uv2.y}});
    }
}


namespace Gx
{
    Text::Text(const Font& font, const sf::String& string, const unsigned int characterSize) :
        m_string             (string),
        m_font               (&font),
        m_characterHeight    (characterSize),
        m_geometryNeedUpdate (true)
    {
    }

    void Text::SetString(const sf::String& string)
    {
        if (m_string != string)
        {
            m_string = string;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetFont(const Font& font)
    {
        if (m_font != &font)
        {
            m_font = &font;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetMasked(const bool masked)
    {
        if (m_masked != masked)
        {
            m_masked = masked;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetStyle(const std::uint32_t style)
    {
        if (m_style != style)
        {
            m_style = style;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetCharacterSize(const unsigned int size)
    {
        if (m_characterHeight != size || m_characterWidth != 0)
        {
            m_characterHeight = size;
            m_characterWidth  = 0;

            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetCharacterWidth(const unsigned int width)
    {
        if (m_characterWidth != width)
        {
            m_characterWidth = width;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetCharacterHeight(unsigned int height)
    {
        if (m_characterHeight != height)
        {
            m_characterHeight = height;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetLetterSpacing(const float spacing)
    {
        if (m_letterSpacing != spacing)
        {
            m_letterSpacing = spacing;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetItalicShear(const sf::Angle& italicSheer)
    {
        m_italicShear = italicSheer;
    }

    void Text::SetLineSpacing(const float spacing)
    {
        if (m_lineSpacing != spacing)
        {
            m_lineSpacing = spacing;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetColor(const sf::Color& color)
    {
        if (color != m_fillColor || !m_colorMap.empty())
        {
            m_fillColor = color;
            m_colorMap.clear();

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!m_geometryNeedUpdate)
            {
                for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
                    m_vertices[i].color = m_fillColor;
            }
        }
    }

    void Text::SetColor(const sf::Color& color, const size_t index)
    {
        if (const auto iterator = m_colorMap.find(index); iterator == m_colorMap.end() || (iterator != m_colorMap.end() && iterator->second != color))
        {
            m_colorMap[index] = color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry need to be updated anyway, we can skip this step)
            if (!m_geometryNeedUpdate)
            {
                if (!m_masked && (m_string[index] == U' ' || m_string[index] == U'\n' && m_string[index] == U'\t'))
                    return;

                size_t start = 0;
                for (size_t i = 0; i < index; ++i)
                {
                    if (m_masked || (m_string[i] != U' ' && m_string[i] != U'\n' && m_string[i] != U'\t'))
                        start++;
                }

                for (std::size_t i = start * 6; i < (start * 6) + 6; ++i)
                {
                    if (i >= m_vertices.getVertexCount())
                        break;

                    m_vertices[i].color = color;
                }
            }
        }
    }

    void Text::SetOutlineColor(const sf::Color& color)
    {
        if (color != m_outlineColor)
        {
            m_outlineColor = color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!m_geometryNeedUpdate)
            {
                for (std::size_t i = 0; i < m_outlineVertices.getVertexCount(); ++i)
                    m_outlineVertices[i].color = m_outlineColor;
            }
        }
    }

    void Text::SetOutlineThickness(const float thickness)
    {
        if (thickness != m_outlineThickness)
        {
            m_outlineThickness = thickness;
            m_geometryNeedUpdate = true;
        }
    }

    void Text::SetOutlineOffset(const sf::Vector2f& offset)
    {
        m_outlineOffset = offset;
    }

    const sf::String& Text::GetString() const
    {
        return m_string;
    }

    const Font* Text::GetFont() const
    {
        return m_font;
    }

    bool Text::IsMasked() const
    {
        return m_masked;
    }

    std::uint32_t Text::GetStyle() const
    {
        return m_style;
    }

    unsigned int Text::GetCharacterSize() const
    {
        return m_characterHeight;
    }

    unsigned int Text::GetCharacterWidth() const
    {
        return m_characterWidth;
    }

    unsigned int Text::GetCharacterHeight() const
    {
        return m_characterHeight;
    }

    float Text::GetLetterSpacing() const
    {
        return m_letterSpacing;
    }

    float Text::GetLineSpacing() const
    {
        return m_lineSpacing;
    }

    sf::Angle Text::GetItalicShear() const
    {
        return m_italicShear;
    }

    const sf::Color& Text::GetColor() const
    {
        return m_fillColor;
    }

    const sf::Color& Text::GetOutlineColor() const
    {
        return m_outlineColor;
    }

    float Text::GetOutlineThickness() const
    {
        return m_outlineThickness;
    }

    sf::Vector2f Text::GetOutlineOffset() const
    {
        return m_outlineOffset;
    }

    sf::Vector2f Text::FindCharacterPosition(std::size_t index) const
    {
        // Make sure that we have a valid font
        if (!m_font)
            return sf::Vector2f();

        // Adjust the index if it's out of range
        if (index > m_string.getSize())
            index = m_string.getSize();

        // Precompute the variables needed by the algorithm
        const bool  isBold          = m_style & static_cast<std::uint32_t>(Bold);
        const float whitespaceWidth = m_font->GetGlyph(U' ', m_characterWidth, m_characterHeight, isBold, 0).advance;
        const float lineSpacing     = m_font->GetLineSpacing(m_characterWidth, m_characterHeight) + m_lineSpacing;

        // Compute the position
        sf::Vector2f position;
        std::uint32_t prevChar = 0;
        for (std::size_t i = 0; i < index; ++i)
        {
            std::uint32_t curChar = m_string[i];
            if (m_masked)
                curChar = U'\u25CF';

            // Apply the kerning offset
            position.x += m_font->GetKerning(prevChar, curChar, m_characterWidth, m_characterHeight, isBold);
            prevChar = curChar;

            // Handle special characters
            switch (curChar)
            {
                case ' ':  position.x += whitespaceWidth;             continue;
                case '\t': position.x += whitespaceWidth * 4;         continue;
                case '\n': position.y += lineSpacing; position.x = 0; continue;
                default: break;
            }

            // For regular characters, add the advance offset of the glyph
            position.x += m_font->GetGlyph(curChar, m_characterWidth, m_characterHeight, isBold, 0).advance + m_letterSpacing;
        }

        // Transform the position to global coordinates
        return GetTransform().transformPoint(position);
    }

    sf::FloatRect Text::GetLocalBounds() const
    {
        EnsureGeometryUpdate();

        return m_bounds;
    }

    sf::FloatRect Text::GetGlobalBounds() const
    {
        return GetTransform().transformRect(GetLocalBounds());
    }

    RenderStates Text::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_font)
        {
            EnsureGeometryUpdate();

            states.transform *= GetTransform();
            states.texture    = &m_font->GetTexture(m_characterWidth, m_characterHeight);

            // Only draw the outline if there is something to draw
            if (m_outlineThickness != 0)
                surface.Render(m_outlineVertices, states);

            surface.Render(m_vertices, states);
        }

        return RenderableContainer::Render(surface, states);
    }

    void Text::EnsureGeometryUpdate() const
    {
        if (!m_font)
            return;

        // Do nothing, if geometry has not changed and the font texture has not changed
        const auto cacheId = m_font->GetTexture(m_characterWidth, m_characterHeight).*Get(CacheID());
        if (!m_geometryNeedUpdate && cacheId == m_fontTextureId)
            return;

        // Save the current fonts texture id
        m_fontTextureId = cacheId;

        // Mark geometry as updated
        m_geometryNeedUpdate = false;

        // Clear the previous geometry
        m_vertices.clear();
        m_outlineVertices.clear();
        m_bounds = sf::FloatRect();

        // No text: nothing to draw
        if (m_string.isEmpty())
            return;

        // Compute values related to the text style
        const bool  isBold             = m_style & Bold;
        const bool  isUnderlined       = m_style & Underlined;
        const bool  isStrikeThrough    = m_style & StrikeThrough;
        const float italicShear        = m_style & Italic ? m_italicShear.asRadians() : 0.f;
        const float underlineOffset    = m_font->GetUnderlinePosition(m_characterWidth, m_characterHeight);
        const float underlineThickness = m_font->GetUnderlineThickness(m_characterWidth, m_characterHeight);

        // Compute the location of the strike through dynamically
        // We use the center point of the lowercase 'x' glyph as the reference
        // We reuse the underline thickness as the thickness of the strike through as well
        const float strikeThroughOffset = m_font->GetGlyph(U'x', m_characterWidth, m_characterHeight, isBold).bounds.getCenter().y;

        // Precompute the variables needed by the algorithm
        const float whitespaceWidth = m_font->GetGlyph(U' ', m_characterWidth, m_characterHeight, isBold, 0).advance;
        const float lineSpacing     = m_font->GetLineSpacing(m_characterWidth, m_characterHeight) + m_lineSpacing;
        float x                     = 0.f;
        float y                     = static_cast<float>(m_characterHeight);

        // Create one quad for each character
        float minX = static_cast<float>(m_characterWidth);
        float minY = static_cast<float>(m_characterHeight);
        float maxX = 0.f;
        float maxY = 0.f;
        std::uint32_t prevChar = 0;
        for (std::size_t i = 0; i < m_string.getSize(); ++i)
        {
            std::uint32_t curChar = m_string[i];
            if (m_masked)
                curChar = U'\u25CF';

            sf::Color fillColor = m_fillColor;
            if (auto color = m_colorMap.find(i); color != m_colorMap.end())
                fillColor = color->second;

            // Skip the \r char to avoid weird graphical issues
            if (curChar == U'\r')
                continue;

            // Apply the kerning offset
            x += m_font->GetKerning(prevChar, curChar, m_characterWidth, m_characterHeight, isBold);

            // If we're using the underlined style and there's a new line, draw a line
            if (isUnderlined && (curChar == U'\n' && prevChar != U'\n'))
            {
                AddLine(m_vertices, x, y, fillColor, underlineOffset, underlineThickness);

                if (m_outlineThickness != 0)
                    AddLine(m_outlineVertices, x, y, m_outlineColor, underlineOffset, underlineThickness, m_outlineThickness);
            }

            // If we're using the strike through style and there's a new line, draw a line across all characters
            if (isStrikeThrough && (curChar == U'\n' && prevChar != U'\n'))
            {
                AddLine(m_vertices, x, y, fillColor, strikeThroughOffset, underlineThickness);

                if (m_outlineThickness != 0)
                    AddLine(m_outlineVertices, x, y, m_outlineColor, strikeThroughOffset, underlineThickness, m_outlineThickness);
            }

            prevChar = curChar;

            // Handle special characters
            if ((curChar == U' ') || (curChar == U'\n') || (curChar == U'\t'))
            {
                // Update the current bounds (min coordinates)
                minX = std::min(minX, x);
                minY = std::min(minY, y);

                switch (curChar)
                {
                    case U' ':  x += whitespaceWidth;     break;
                    case U'\t': x += whitespaceWidth * 4; break;
                    case U'\n': y += lineSpacing; x = 0;  break;
                    default: break;
                }

                // Update the current bounds (max coordinates)
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);

                // Next glyph, no need to create a quad for whitespace
                continue;
            }

            // Apply the outline
            if (m_outlineThickness != 0)
            {
                const sf::Glyph& glyph = m_font->GetGlyph(curChar, m_characterWidth, m_characterHeight, isBold, m_outlineThickness);

                // Add the outline glyph to the vertices
                AddGlyphQuad(m_outlineVertices, sf::Vector2f(x, y), m_outlineColor, glyph, italicShear, m_outlineOffset);
            }

            // Extract the current glyph's description
            const sf::Glyph& glyph = m_font->GetGlyph(curChar, m_characterWidth, m_characterHeight, isBold, 0);

            // Add the glyph to the vertices
            AddGlyphQuad(m_vertices, sf::Vector2f(x, y), fillColor, glyph, italicShear);

            // Update the current bounds
            const float left   = glyph.bounds.position.x;
            const float top    = glyph.bounds.position.y;
            const float right  = glyph.bounds.position.x + glyph.bounds.size.x;
            const float bottom = glyph.bounds.position.y + glyph.bounds.size.y;

            minX = std::min(minX, x + left - italicShear * bottom);
            maxX = std::max(maxX, x + right - italicShear * top);
            minY = std::min(minY, y + top);
            maxY = std::max(maxY, y + bottom);

            // Advance to the next character
            x += glyph.advance + m_letterSpacing;
        }

        // If we're using outline, update the current bounds
        if (m_outlineThickness != 0)
        {
            const float outline = std::abs(std::ceil(m_outlineThickness));
            minX -= outline;
            maxX += outline;
            minY -= outline;
            maxY += outline;
        }

        // If we're using the underlined style, add the last line
        if (isUnderlined && (x > 0))
        {
            AddLine(m_vertices, x, y, m_fillColor, underlineOffset, underlineThickness);

            if (m_outlineThickness != 0)
                AddLine(m_outlineVertices, x, y, m_outlineColor, underlineOffset, underlineThickness, m_outlineThickness);
        }

        // If we're using the strike through style, add the last line across all characters
        if (isStrikeThrough && (x > 0))
        {
            AddLine(m_vertices, x, y, m_fillColor, strikeThroughOffset, underlineThickness);

            if (m_outlineThickness != 0)
                AddLine(m_outlineVertices, x, y, m_outlineColor, strikeThroughOffset, underlineThickness, m_outlineThickness);
        }

        // Update the bounding rectangle
        m_bounds.position.x   = minX;
        m_bounds.position.y    = minY;
        m_bounds.size.x  = maxX - minX;
        m_bounds.size.y = maxY - minY;

        OnGeometryUpdated();
    }

}