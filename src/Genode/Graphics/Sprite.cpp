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
#include <Genode/Graphics/Sprite.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cmath>

namespace Gx
{
    Sprite::Sprite() :
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(BlendMode::Auto)
    {
    }

    Sprite::Sprite(const sf::Texture& texture) :
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(BlendMode::Auto)
    {
        SetTexture(texture);
    }

    Sprite::Sprite(const sf::Texture& texture, const sf::IntRect& rectangle) :
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(BlendMode::Auto)
    {
        SetTexture(texture);
        SetTexCoords(rectangle);
    }

    void Sprite::SetTexture(const sf::Texture& texture, const bool resetRect)
    {
        // Recompute the texture area if requested, or if there was no valid texture & rect before
        if (resetRect || (!m_texture && (m_texcoords == sf::IntRect())))
            SetTexCoords(sf::IntRect({0, 0}, sf::Vector2i(texture.getSize())));

        // Assign the new texture
        m_texture = &texture;
    }

    void Sprite::SetTexCoords(const sf::IntRect& rectangle)
    {
        if (rectangle != m_texcoords)
        {
            m_texcoords = rectangle;
            UpdateVertices();
        }
    }

    void Sprite::SetColor(const sf::Color& color)
    {
        // Update the vertices' color
        for (auto& vertex : m_vertices)
            vertex.color = color;
    }

    const sf::Texture* Sprite::GetTexture() const
    {
        return m_texture;
    }

    const sf::IntRect& Sprite::GetTexCoords() const
    {
        return m_texcoords;
    }

    const sf::Color& Sprite::GetColor() const
    {
        return m_vertices[0].color;
    }

    BlendMode Sprite::GetBlendMode() const
    {
        return m_blendMode;
    }

    void Sprite::SetBlendMode(const BlendMode blendMode)
    {
        m_blendMode = blendMode;
    }

    sf::FloatRect Sprite::GetLocalBounds() const
    {
        const auto width = static_cast<float>(std::abs(m_texcoords.size.x));
        const auto height = static_cast<float>(std::abs(m_texcoords.size.y));

        return {{0.f, 0.f}, {width, height}};
    }

    sf::FloatRect Sprite::GetGlobalBounds() const
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

    RenderStates Sprite::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform     *= GetTransform();
        states.coordinateType = sf::CoordinateType::Pixels;
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

        if (m_texture)
        {
            states.texture = m_texture;
            surface.Render(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TriangleStrip, states);
        }

        return RenderableContainer::Render(surface, states);
    }

    void Sprite::UpdateVertices()
    {
        const auto [position, size] = sf::FloatRect(m_texcoords);

        // Absolute value is used to support negative texture rect sizes
        const sf::Vector2f absSize(std::abs(size.x), std::abs(size.y));

        // Update positions
        m_vertices[0].position = {0.f, 0.f};
        m_vertices[1].position = {0.f, absSize.y};
        m_vertices[2].position = {absSize.x, 0.f};
        m_vertices[3].position = absSize;

        // Update texture coordinates
        m_vertices[0].texCoords = position;
        m_vertices[1].texCoords = position + sf::Vector2f(0.f, size.y);
        m_vertices[2].texCoords = position + sf::Vector2f(size.x, 0.f);
        m_vertices[3].texCoords = position + size;
    }
}
