////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
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

#include <Genode/Graphics/Shape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Err.hpp>
#include <cmath>

namespace
{
    // Compute the normal of a segment
    sf::Vector2f computeNormal(const sf::Vector2f& p1, const sf::Vector2f& p2)
    {
        sf::Vector2f normal(p1.y - p2.y, p2.x - p1.x);
        const float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length != 0.f)
            normal /= length;
        return normal;
    }

    // Compute the dot product of two vectors
    float dotProduct(const sf::Vector2f& p1, const sf::Vector2f& p2)
    {
        return p1.x * p2.x + p1.y * p2.y;
    }
}

namespace Gx
{
    Shape::Shape() :
        Shape(sf::PrimitiveType::TriangleFan)
    {
    }

    Shape::Shape(const sf::PrimitiveType primitiveType) :
        m_texture(nullptr),
        m_textureRect(),
        m_fillColor(255, 255, 255),
        m_outlineColor(255, 255, 255),
        m_outlineThickness(0),
        m_vertices(primitiveType),
        m_outlineVertices(sf::PrimitiveType::TriangleStrip),
        m_insideBounds(),
        m_bounds()
    {
    }

    void Shape::SetTexture(const sf::Texture& texture, const bool resetRect)
    {
        // Recompute the texture area if requested, or if there was no texture & rect before
        if (resetRect || (!m_texture && (m_textureRect == sf::IntRect())))
            SetTexCoords(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)));

        // Assign the new texture
        m_texture = &texture;
    }

    const sf::Texture* Shape::GetTexture() const
    {
        return m_texture;
    }

    void Shape::SetTexCoords(const sf::IntRect& rect)
    {
        m_textureRect = rect;
        UpdateTexCoords();
    }

    const sf::IntRect& Shape::GetTexCoords() const
    {
        return m_textureRect;
    }

    void Shape::SetOutlineColor(const sf::Color& color)
    {
        m_outlineColor = color;
        UpdateOutlineColors();
    }

    const sf::Color& Shape::GetOutlineColor() const
    {
        return m_outlineColor;
    }

    void Shape::SetOutlineThickness(const float thickness)
    {
        m_outlineThickness = thickness;
        Update(); // recompute everything because the whole shape must be offset
    }

    float Shape::GetOutlineThickness() const
    {
        return m_outlineThickness;
    }

    const sf::Color& Shape::GetColor() const
    {
        return m_fillColor;
    }

    const sf::Color& Shape::GetColor(const unsigned int index) const
    {
        if (const auto it = m_colorMap.find(index); it != m_colorMap.end())
           return it->second;

        return m_fillColor;
    }

    void Shape::SetColor(const sf::Color& color)
    {
        m_fillColor = color;
        UpdateFillColors();
    }

    void Shape::SetColor(const unsigned int index, const sf::Color& color)
    {
        m_colorMap[index] = color;
    }

    sf::Vector2f Shape::GetGeometricCenter() const
    {
        switch (const auto count = GetPointCount())
        {
            case 0:
                assert(false && "Cannot calculate geometric center of shape with no points");
            return sf::Vector2f{};
            case 1:
                return GetPoint(0);
            case 2:
                return (GetPoint(0) + GetPoint(1)) / 2.f;
            default: // more than two points
                sf::Vector2f centroid;
                float    twiceArea = 0;

                auto previousPoint = GetPoint(count - 1);
                for (std::size_t i = 0; i < count; ++i)
                {
                    const auto  currentPoint = GetPoint(i);
                    const float product      = previousPoint.cross(currentPoint);
                    twiceArea += product;
                    centroid += (currentPoint + previousPoint) * product;

                    previousPoint = currentPoint;
                }

                if (twiceArea != 0.f)
                {
                    return centroid / 3.f / twiceArea;
                }

                // Fallback for no area - find the center of the bounding box
                auto minPoint = GetPoint(0);
                auto maxPoint = minPoint;
                for (std::size_t i = 1; i < count; ++i)
                {
                    const auto currentPoint = GetPoint(i);
                    minPoint.x              = std::min(minPoint.x, currentPoint.x);
                    maxPoint.x              = std::max(maxPoint.x, currentPoint.x);
                    minPoint.y              = std::min(minPoint.y, currentPoint.y);
                    maxPoint.y              = std::max(maxPoint.y, currentPoint.y);
                }
                return (maxPoint + minPoint) / 2.f;
        }
    }

    sf::FloatRect Shape::GetLocalBounds() const
    {
        return m_bounds;
    }

    sf::FloatRect Shape::GetGlobalBounds() const
    {
        auto parent    = GetParent();
        auto transform = sf::Transform::Identity;
        while (parent)
        {
            transform *= parent->GetTransform();
            parent = parent->GetParent();
        }

        transform *= GetTransform();
        return transform.transformRect(GetLocalBounds());
    }

    void Shape::Update()
    {
        // Get the total number of points of the shape
        const std::size_t count = GetPointCount();
        if (count < 3)
        {
            m_vertices.resize(0);
            m_outlineVertices.resize(0);
            return;
        }

        m_vertices.resize(count + 2); // + 2 for center and repeated first point

        // Position
        for (std::size_t i = 0; i < count; ++i)
            m_vertices[i + 1].position = GetPoint(i);
        m_vertices[count + 1].position = m_vertices[1].position;

        // Update the bounding rectangle
        m_vertices[0] = m_vertices[1]; // so that the result of GetBounds() is correct
        m_insideBounds = m_vertices.getBounds();

        // Compute the center and make it the first vertex
        m_vertices[0].position.x = m_insideBounds.position.x + m_insideBounds.size.x / 2;
        m_vertices[0].position.y = m_insideBounds.position.y + m_insideBounds.size.y / 2;

        // sf::Color
        UpdateFillColors();

        // sf::Texture coordinates
        UpdateTexCoords();

        // Outline
        UpdateOutline();
    }

    RenderStates Shape::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();

        // Render the inside
        states.texture = m_texture;
        surface.Render(m_vertices, states);

        // Render the outline
        if (m_outlineThickness != 0)
        {
            states.texture = nullptr;
            surface.Render(m_outlineVertices, states);
        }

        return RenderableContainer::Render(surface, states);
    }

    void Shape::UpdateFillColors()
    {
        for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
            m_vertices[i].color = GetColor(i);
    }

    void Shape::UpdateTexCoords()
    {
        const auto convertedTextureRect = sf::FloatRect(m_textureRect);

        for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
        {
            const float xratio = m_insideBounds.size.x > 0 ? (m_vertices[i].position.x - m_insideBounds.position.x) / m_insideBounds.size.x : 0;
            const float yratio = m_insideBounds.size.y > 0 ? (m_vertices[i].position.y - m_insideBounds.position.y) / m_insideBounds.size.y : 0;
            m_vertices[i].texCoords.x = convertedTextureRect.position.x + convertedTextureRect.size.x * xratio;
            m_vertices[i].texCoords.y = convertedTextureRect.position.y + convertedTextureRect.size.y * yratio;
        }
    }

    void Shape::UpdateOutline()
    {
        // Return if there is no outline
        if (m_outlineThickness == 0.f)
        {
            m_outlineVertices.clear();
            m_bounds = m_insideBounds;
            return;
        }

        const std::size_t count = m_vertices.getVertexCount() - 2;
        m_outlineVertices.resize((count + 1) * 2);

        for (std::size_t i = 0; i < count; ++i)
        {
            const std::size_t index = i + 1;

            // Get the two segments shared by the current point
            sf::Vector2f p0 = (i == 0) ? m_vertices[count].position : m_vertices[index - 1].position;
            sf::Vector2f p1 = m_vertices[index].position;
            sf::Vector2f p2 = m_vertices[index + 1].position;

            // Compute their normal
            sf::Vector2f n1 = computeNormal(p0, p1);
            sf::Vector2f n2 = computeNormal(p1, p2);

            // Make sure that the normals point towards the outside of the shape
            // (this depends on the order in which the points were defined)
            if (dotProduct(n1, m_vertices[0].position - p1) > 0)
                n1 = -n1;
            if (dotProduct(n2, m_vertices[0].position - p1) > 0)
                n2 = -n2;

            // Combine them to get the extrusion direction
            const float factor = 1.f + (n1.x * n2.x + n1.y * n2.y);
            sf::Vector2f normal = (n1 + n2) / factor;

            // Update the outline points
            m_outlineVertices[i * 2 + 0].position = p1;
            m_outlineVertices[i * 2 + 1].position = p1 + normal * m_outlineThickness;
        }

        // Duplicate the first point at the end, to close the outline
        m_outlineVertices[count * 2 + 0].position = m_outlineVertices[0].position;
        m_outlineVertices[count * 2 + 1].position = m_outlineVertices[1].position;

        // Update outline colors
        UpdateOutlineColors();

        // Update the shape's bounds
        m_bounds = m_outlineVertices.getBounds();
    }

    void Shape::UpdateOutlineColors()
    {
        for (std::size_t i = 0; i < m_outlineVertices.getVertexCount(); ++i)
            m_outlineVertices[i].color = m_outlineColor;
    }

}
