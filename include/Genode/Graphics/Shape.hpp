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

#pragma once

#include <Genode/Entities/Colorable.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include <unordered_map>

namespace Gx
{
    ////////////////////////////////////////////////////////////
    /// @brief Base class for textured shapes with outline
    ////////////////////////////////////////////////////////////
    class Shape : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer,
                  public virtual InputableContainer, public virtual Colorable
    {
    public:
        ////////////////////////////////////////////////////////////
        /// @brief Change the source texture of the shape
        ///
        /// The @a `texture` argument refers to a texture that must
        /// exist as long as the shape uses it. Indeed, the shape
        /// doesn't store its own copy of the texture, but rather keeps
        /// a pointer to the one that you passed to this function.
        /// If the source texture is destroyed and the shape tries to
        /// use it, the behavior is undefined.
        /// @a `texture` can be a null pointer to disable texturing.
        /// If @a `resetRect` is `true`, the `TextureRect` property of
        /// the shape is automatically adjusted to the size of the new
        /// texture. If it is `false`, the texture rect is left unchanged.
        ///
        /// @param texture   New texture
        /// @param resetRect Should the texture rect be reset to the size of the new texture?
        ///
        /// @see `GetTexture`, `SetTextureRect`
        ////////////////////////////////////////////////////////////
        void SetTexture(const sf::Texture& texture, bool resetRect = false);

        ////////////////////////////////////////////////////////////
        /// @brief Set the sub-rectangle of the texture that the shape will display
        ///
        /// The texture rect is useful when you don't want to display
        /// the whole texture, but rather a part of it.
        /// By default, the texture rect covers the entire texture.
        ///
        /// @param rect Rectangle defining the region of the texture to display
        ///
        /// @see `GetTextureCoords`, `SetTexture`
        ////////////////////////////////////////////////////////////
        void SetTexCoords(const sf::IntRect& rect);

        ////////////////////////////////////////////////////////////
        /// @brief Set the fill color of the shape
        ///
        /// This color is modulated (multiplied) with the shape's
        /// texture if any. It can be used to colorize the shape,
        /// or change its global opacity.
        /// You can use `sf::Color::Transparent to make the inside of
        /// the shape transparent, and have the outline alone.
        /// By default, the shape's fill color is opaque white.
        ///
        /// @param color New color of the shape
        ///
        /// @see `GetColor`, `SetOutlineColor`
        ////////////////////////////////////////////////////////////
        void SetColor(const sf::Color& color) override;

        ////////////////////////////////////////////////////////////
        /// @brief Set the fill color of the shape of specific point
        ///
        /// This color is modulated (multiplied) with the shape's
        /// texture if any. It can be used to colorize the shape,
        /// or change its global opacity.
        /// You can use `sf::Color::Transparent to make the inside of
        /// the shape transparent, and have the outline alone.
        /// By default, the shape's fill color is opaque white.
        ///
        /// @param color New color of the shape
        /// @param index The point index
        ///
        /// @see `GetColor`, `SetOutlineColor`
        ////////////////////////////////////////////////////////////
        void SetColor(unsigned int index, const sf::Color& color);

        ////////////////////////////////////////////////////////////
        /// @brief Set the outline color of the shape
        ///
        /// By default, the shape's outline color is opaque white.
        ///
        /// @param color New outline color of the shape
        ///
        /// @see `GetOutlineColor`, `SetColor`
        ////////////////////////////////////////////////////////////
        void SetOutlineColor(const sf::Color& color);

        ////////////////////////////////////////////////////////////
        /// @brief Set the thickness of the shape's outline
        ///
        /// Note that negative values are allowed (so that the outline
        /// expands towards the center of the shape), and using zero
        /// disables the outline.
        /// By default, the outline thickness is 0.
        ///
        /// @param thickness New outline thickness
        ///
        /// @see `GetOutlineThickness`
        ////////////////////////////////////////////////////////////
        void SetOutlineThickness(float thickness);

        ////////////////////////////////////////////////////////////
        /// @brief Get the source texture of the shape
        ///
        /// If the shape has no source texture, a `nullptr` is returned.
        /// The returned pointer is const, which means that you can't
        /// modify the texture when you retrieve it with this function.
        ///
        /// @return Pointer to the shape's texture
        ///
        /// @see `SetTexture`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Texture* GetTexture() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the sub-rectangle of the texture displayed by the shape
        ///
        /// @return Texture rectangle of the shape
        ///
        /// @see `SetTextureRect`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::IntRect& GetTexCoords() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the fill color of the shape
        ///
        /// @return Fill color of the shape
        ///
        /// @see `SetColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetColor() const override;

        ////////////////////////////////////////////////////////////
        /// @brief Get the fill color of a specific point of the shape
        ///
        /// @param index The point index
        ///
        /// @return Fill color of the point shape
        ///
        /// @see `SetColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetColor(unsigned int index) const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the outline color of the shape
        ///
        /// @return Outline color of the shape
        ///
        /// @see `SetOutlineColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetOutlineColor() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the outline thickness of the shape
        ///
        /// @return Outline thickness of the shape
        ///
        /// @see `SetOutlineThickness`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] float GetOutlineThickness() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the total number of points of the shape
        ///
        /// @return Number of points of the shape
        ///
        /// @see `GetPoint`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] virtual std::size_t GetPointCount() const = 0;

        ////////////////////////////////////////////////////////////
        /// @brief Get a point of the shape
        ///
        /// The returned point is in local coordinates, that is,
        /// the shape's transforms (position, rotation, scale) are
        /// not taken into account.
        /// The result is undefined if @a `index` is out of the valid range.
        ///
        /// @param index Index of the point to get, in range [0 .. GetPointCount() - 1]
        ///
        /// @return index-th point of the shape
        ///
        /// @see `GetPointCount`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] virtual sf::Vector2f GetPoint(std::size_t index) const = 0;

        ////////////////////////////////////////////////////////////
        /// @brief Get the geometric center of the shape
        ///
        /// The returned point is in local coordinates, that is,
        /// the shape's transforms (position, rotation, scale) are
        /// not taken into account.
        ///
        /// @return The geometric center of the shape
        ////////////////////////////////////////////////////////////
        [[nodiscard]] virtual sf::Vector2f GetGeometricCenter() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the local bounding rectangle of the entity
        ///
        /// The returned rectangle is in local coordinates, which means
        /// that it ignores the transformations (translation, rotation,
        /// scale, ...) that are applied to the entity.
        /// In other words, this function returns the bounds of the
        /// entity in the entity's coordinate system.
        ///
        /// @return Local bounding rectangle of the entity
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::FloatRect GetLocalBounds() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the global (non-minimal) bounding rectangle of the entity
        ///
        /// The returned rectangle is in global coordinates, which means
        /// that it takes into account the transformations (translation,
        /// rotation, scale, ...) that are applied to the entity.
        /// In other words, this function returns the bounds of the
        /// shape in the global 2D world's coordinate system.
        ///
        /// This function does not necessarily return the @a `minimal`
        /// bounding rectangle. It merely ensures that the returned
        /// rectangle covers all the vertices (but possibly more).
        /// This allows for a fast approximation of the bounds as a
        /// first check; you may want to use more precise checks
        /// on top of that.
        ///
        /// @return Global bounding rectangle of the entity
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::FloatRect GetGlobalBounds() const;

    protected:
        ////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ////////////////////////////////////////////////////////////
        Shape();

        ////////////////////////////////////////////////////////////
        /// @brief Construct Shape with specified primitive type
        ///
        /// @param  primitiveType The primitive type of shape vertices
        ////////////////////////////////////////////////////////////
        explicit Shape(sf::PrimitiveType primitiveType);

        ////////////////////////////////////////////////////////////
        /// @brief Recompute the internal geometry of the shape
        ///
        /// This function must be called by the derived class every time
        /// the shape's points change (i.e. the result of either
        /// GetPointCount or GetPoint is different).
        ////////////////////////////////////////////////////////////
        void Update();

    private:
        using ColorMap = std::unordered_map<unsigned int, sf::Color>;

        ////////////////////////////////////////////////////////////
        /// @brief Render the shape to a render surface
        ///
        /// @param surface Render surface to draw to
        /// @param states Current render states
        ////////////////////////////////////////////////////////////
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        ////////////////////////////////////////////////////////////
        /// @brief Update the fill vertices' color
        ////////////////////////////////////////////////////////////
        void UpdateFillColors();

        ////////////////////////////////////////////////////////////
        /// @brief Update the fill vertices' texture coordinates
        ////////////////////////////////////////////////////////////
        void UpdateTexCoords();

        ////////////////////////////////////////////////////////////
        /// @brief Update the outline vertices' position
        ////////////////////////////////////////////////////////////
        void UpdateOutline();

        ////////////////////////////////////////////////////////////
        /// @brief Update the outline vertices' color
        ////////////////////////////////////////////////////////////
        void UpdateOutlineColors();

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        const sf::Texture* m_texture{};                                       //!< Texture of the shape
        sf::IntRect        m_textureRect;                                     //!< Rectangle defining the area of the source texture to display
        sf::Color          m_fillColor{sf::Color::White};                     //!< Fill color
        sf::Color          m_outlineColor{sf::Color::White};                  //!< Outline color
        ColorMap           m_colorMap{};                                      //!< Point fill colors
        float              m_outlineThickness{};                              //!< Thickness of the shape's outline
        sf::VertexArray    m_vertices{sf::PrimitiveType::TriangleFan};        //!< Vertex array containing the fill geometry
        sf::VertexArray    m_outlineVertices{sf::PrimitiveType::TriangleFan}; //!< Vertex array containing the outline geometry
        sf::FloatRect      m_insideBounds;                                    //!< Bounding rectangle of the inside (fill)
        sf::FloatRect      m_bounds;                                          //!< Bounding rectangle of the whole shape (outline + fill)
    };

}
