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

#ifndef GENODE_GRAPHICS_SPRITE_HPP
#define GENODE_GRAPHICS_SPRITE_HPP

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/Entities/Colorable.hpp>
#include <Genode/Graphics/BlendMode.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <memory>

namespace Gx
{
    class Sprite : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer, public virtual Colorable
    {
    public:
        Sprite();
        explicit Sprite(const sf::Texture& texture);
        Sprite(const sf::Texture& texture, const sf::IntRect& rectangle);

        void SetTexture(const sf::Texture& texture, bool resetRect = false);
        void SetTexCoords(const sf::IntRect& rectangle);
        void SetColor(const sf::Color& color) override;

        const sf::Texture* GetTexture() const;
        const sf::IntRect& GetTexCoords() const;
        const sf::Color& GetColor() const override;

        BlendMode GetBlendMode() const;
        void SetBlendMode(Gx::BlendMode blendMode);

        virtual sf::FloatRect GetLocalBounds() const;
        sf::FloatRect GetGlobalBounds() const;

    protected:
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

    private:
        void UpdatePositions();
        void UpdateTexCoords();

        std::array<sf::Vertex, 4> m_vertices;
        const sf::Texture* m_texture;
        sf::IntRect m_texcoords;
        BlendMode m_blendMode;
    };

}

#endif