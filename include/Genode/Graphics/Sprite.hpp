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

#ifndef GENODE_SPRITE_HPP
#define GENODE_SPRITE_HPP

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Entities.hpp>

#include <memory>

namespace Gx
{
    typedef std::shared_ptr<const sf::Texture> TextureHandle;

    class Sprite : virtual public Node, public RenderableContainer, public UpdatableContainer, public InputableContainer, public Colorable
    {
    public:
        Sprite();
        explicit Sprite(const sf::Texture& texture);
        Sprite(const sf::Texture& texture, const sf::IntRect& rectangle);

        explicit Sprite(TextureHandle texture);
        Sprite(TextureHandle texture, const sf::IntRect& rectangle);

        void SetTexture(const sf::Texture& texture, bool resetRect = false);
        void SetTexture(TextureHandle texture, bool resetRect = false);
        void SetTexCoords(const sf::IntRect& rectangle);
        virtual void SetColor(const sf::Color& color);

        const sf::Texture* GetTexture() const;
        const sf::IntRect& GetTexCoords() const;
        virtual const sf::Color& GetColor() const;

        sf::FloatRect GetLocalBounds() const;
        sf::FloatRect GetGlobalBounds() const;

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        void UpdatePositions();
        void UpdateTexCoords();

        sf::Vertex  m_vertices[4];
        TextureHandle  m_texture;     
        sf::IntRect m_texcoords;
    };

}

#endif