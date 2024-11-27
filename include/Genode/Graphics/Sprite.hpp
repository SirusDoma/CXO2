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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Genode/Entities/Colorable.hpp>
#include <Genode/Graphics/BlendMode.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <memory>
#include <SFML/Graphics/Sprite.hpp>

namespace Gx
{
    ////////////////////////////////////////////////////////////
    /// @brief Renderable representation of a texture, with its
    ///        own transformations, color, etc.
    ////////////////////////////////////////////////////////////
    class Sprite : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer,
                   public virtual InputableContainer, public virtual Colorable
    {
    public:
        ////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ////////////////////////////////////////////////////////////
        Sprite();

        ////////////////////////////////////////////////////////////
        /// @brief Construct the sprite from a source texture
        ///
        /// @param texture Source texture
        ///
        /// @see `SetTexture`
        ////////////////////////////////////////////////////////////
        explicit Sprite(const sf::Texture& texture);

        ////////////////////////////////////////////////////////////
        /// @brief Construct the sprite from a sub-rectangle of a source texture
        ///
        /// @param texture Source texture
        /// @param rectangle Sub-rectangle of the texture
        ///
        /// @see `SetTexture`
        ////////////////////////////////////////////////////////////
        Sprite(const sf::Texture& texture, const sf::IntRect& rectangle);

        ////////////////////////////////////////////////////////////
        /// @brief Change the source texture of the sprite
        ///
        /// @param texture New texture
        /// @param resetRect Should the texture rectangle be reset to the size of the new texture?
        ///
        /// @see `GetTexture`
        ////////////////////////////////////////////////////////////
        void SetTexture(const sf::Texture& texture, bool resetRect = false);

        ////////////////////////////////////////////////////////////
        /// @brief Set the sub-rectangle of the texture that the sprite will display
        ///
        /// @param rectangle Rectangle defining the region of the texture to display
        ///
        /// @see `GetTexCoords`
        ////////////////////////////////////////////////////////////
        void SetTexCoords(const sf::IntRect& rectangle);

        ////////////////////////////////////////////////////////////
        /// @brief Set the global color of the sprite
        ///
        /// @param color New color
        ///
        /// @see `GetColor`
        ////////////////////////////////////////////////////////////
        void SetColor(const sf::Color& color) override;

        ////////////////////////////////////////////////////////////
        /// @brief Set the blend mode of the sprite
        ///
        /// @param blendMode New blend mode for the sprite
        ///
        /// @see `GetBlendMode`
        ////////////////////////////////////////////////////////////
        void SetBlendMode(Gx::BlendMode blendMode);

        ////////////////////////////////////////////////////////////
        /// @brief Get the source texture of the sprite
        ///
        /// @return Pointer to the sprite's texture
        ///
        /// @see `SetTexture`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Texture* GetTexture() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the sub-rectangle of the texture displayed by the sprite
        ///
        /// @return `IntRect` object representing the texture coordinates of the sprite
        ///
        /// @see `SetTexCoords`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::IntRect& GetTexCoords() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the global color of the sprite
        ///
        /// @return Global color of the sprite
        ///
        /// @see `SetColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetColor() const override;

        ////////////////////////////////////////////////////////////
        /// @brief Get the blend mode of the sprite
        ///
        /// @return `BlendMode` object representing the blend mode of the sprite
        ///
        /// @see `SetBlendMode`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] BlendMode GetBlendMode() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the local bounding rectangle of the entity
        ///
        /// @return Local bounding rectangle of the entity
        ///
        /// @see `GetGlobalBounds`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] virtual sf::FloatRect GetLocalBounds() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the global bounding rectangle of the entity
        ///
        /// @return Global bounding rectangle of the entity
        ///
        /// @see `GetLocalBounds`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] virtual sf::FloatRect GetGlobalBounds() const;

    protected:
        ////////////////////////////////////////////////////////////
        /// @brief Render the sprite to a render surface
        ///
        /// @param surface Render surface to draw to
        /// @param states Current render states
        ////////////////////////////////////////////////////////////
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

    private:
        ////////////////////////////////////////////////////////////
        /// @brief Update the vertices' texture coordinates
        ////////////////////////////////////////////////////////////
        void UpdateVertices();

        std::array<sf::Vertex, 4> m_vertices; ///< Vertices defining the sprite's geometry
        const sf::Texture* m_texture;         ///< Source texture of the sprite
        sf::IntRect m_texcoords;              ///< Sub-rectangle of the texture displayed by the sprite
        BlendMode m_blendMode;                ///< Blend mode used for rendering
    };
}
