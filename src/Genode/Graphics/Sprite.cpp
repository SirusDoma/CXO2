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

#include <Genode/Graphics/Sprite.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cstdlib>

namespace Gx
{
    Sprite::Sprite() :
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(Gx::BlendMode::Auto)
    {
    }

    Sprite::Sprite(const sf::Texture& texture) :
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(Gx::BlendMode::Auto)
    {
        SetTexture(texture);
    }

    Sprite::Sprite(const sf::Texture& texture, const sf::IntRect& rectangle) :
        m_texture(nullptr),
        m_texcoords(),
        m_blendMode(Gx::BlendMode::Auto)
    {
        SetTexture(texture);
        SetTexCoords(rectangle);
    }

    void Sprite::SetTexture(const sf::Texture& texture, const bool resetRect)
    {
        // Recompute the texture area if requested, or if there was no valid texture & rect before
        if (resetRect || (!m_texture && (m_texcoords == sf::IntRect())))
            SetTexCoords(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(static_cast<int>(texture.getSize().x), static_cast<int>(texture.getSize().y))));

        // Assign the new texture
        m_texture = &texture;
    }

    void Sprite::SetTexCoords(const sf::IntRect& rectangle)
    {
        if (rectangle != m_texcoords)
        {
            m_texcoords = rectangle;
            UpdatePositions();
            UpdateTexCoords();
        }
    }

    void Sprite::SetColor(const sf::Color& color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
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

    void Sprite::SetBlendMode(const Gx::BlendMode blendMode)
    {
        m_blendMode = blendMode;
    }

    sf::FloatRect Sprite::GetLocalBounds() const
    {
        const auto width = static_cast<float>(std::abs(m_texcoords.width));
        const auto height = static_cast<float>(std::abs(m_texcoords.height));

        return {sf::Vector2f(0.f, 0.f), sf::Vector2f(width, height)};
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

    RenderStates Sprite::Render(sf::RenderTarget& target, RenderStates states) const
    {
        if (m_texture)
        {
            states.texture = m_texture;
            states.transform *= GetTransform();
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

            target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, states);
        }

        return RenderableContainer::Render(target, states);
    }

    void Sprite::UpdatePositions()
    {
        const sf::FloatRect bounds = GetLocalBounds();

        m_vertices[0].position = sf::Vector2f(0, 0);
        m_vertices[1].position = sf::Vector2f(0, bounds.height);
        m_vertices[2].position = sf::Vector2f(bounds.width, 0);
        m_vertices[3].position = sf::Vector2f(bounds.width, bounds.height);
    }

    void Sprite::UpdateTexCoords()
    {
        const auto left = static_cast<float>(m_texcoords.left);
        const float right = left + static_cast<float>(m_texcoords.width);
        const auto top = static_cast<float>(m_texcoords.top);
        const float bottom = top + static_cast<float>(m_texcoords.height);

        m_vertices[0].texCoords = sf::Vector2f(left, top);
        m_vertices[1].texCoords = sf::Vector2f(left, bottom);
        m_vertices[2].texCoords = sf::Vector2f(right, top);
        m_vertices[3].texCoords = sf::Vector2f(right, bottom);
    }
}
