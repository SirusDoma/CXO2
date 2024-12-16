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
#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/Entities/Colorable.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System.hpp>

#include <unordered_map>
#include <memory>

namespace Gx
{
    class Font;
    typedef std::shared_ptr<const Font> FontHandle;

    ////////////////////////////////////////////////////////////
    /// @brief Graphical text that can be drawn to a render target
    ////////////////////////////////////////////////////////////
    class Text : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer,
                 public virtual InputableContainer, public virtual Colorable
    {
    public:
        ////////////////////////////////////////////////////////////
        /// @brief Enumeration of the string drawing styles
        ////////////////////////////////////////////////////////////
        enum Style
        {
            Regular       = 0,      //!< Regular characters, no style
            Bold          = 1 << 0, //!< Bold characters
            Italic        = 1 << 1, //!< Italic characters
            Underlined    = 1 << 2, //!< Underlined characters
            StrikeThrough = 1 << 3  //!< Strike through characters
        };

        ////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ////////////////////////////////////////////////////////////
        Text() = default;

        ////////////////////////////////////////////////////////////
        /// @brief Construct the text from a string, font and size
        ///
        /// Note that if the used font is a bitmap font, it is not
        /// scalable, thus not all requested sizes will be available
        /// to use. This needs to be taken into consideration when
        /// setting the character size. If you need to display text
        /// of a certain size, make sure the corresponding bitmap
        /// font that supports that size is used.
        ///
        /// @param string         Text assigned to the string
        /// @param font           Font used to draw the string
        /// @param characterSize  Base size of characters, in pixels
        ////////////////////////////////////////////////////////////
        explicit Text(const Font& font, sf::String  string = "", unsigned int characterSize = 30);

        ////////////////////////////////////////////////////////////
        /// @brief Disallow construction from a temporary font
        ////////////////////////////////////////////////////////////
        explicit Text(const Font&& font, const sf::String& string = "", unsigned int characterSize = 30) = delete;

        ////////////////////////////////////////////////////////////
        /// @brief Set the text's string
        ///
        /// The @a `string` argument is a `sf::String`, which can
        /// automatically be constructed from standard string types.
        /// So, the following calls are all valid:
        /// @code
        /// text.SetString("hello");
        /// text.SetString(L"hello");
        /// text.SetString(std::string("hello"));
        /// text.SetString(std::wstring(L"hello"));
        /// @endcode
        /// A text's string is empty by default.
        ///
        /// @param string New string
        ///
        /// @see `GetString`
        ////////////////////////////////////////////////////////////
        void SetString(const sf::String& string);

        ////////////////////////////////////////////////////////////
        /// @brief Set the text's font
        ///
        /// The @a `font` argument refers to a font that must
        /// exist as long as the text uses it. Indeed, the text
        /// doesn't store its own copy of the font, but rather keeps
        /// a pointer to the one that you passed to this function.
        /// If the font is destroyed and the text tries to
        /// use it, the behavior is undefined.
        ///
        /// @param font New font
        ///
        /// @see `GetFont`
        ////////////////////////////////////////////////////////////
        void SetFont(const Font& font);

        ////////////////////////////////////////////////////////////
        /// @brief Specify whether the string should be displayed as masked string.
        ///
        /// A text's string is empty by default.
        ///
        /// @param masked A value determines whether the string should be masked.
        ///
        /// @see `IsMasked`
        ////////////////////////////////////////////////////////////
        void SetMasked(bool masked);

        ////////////////////////////////////////////////////////////
        /// @brief Set the text's style
        ///
        /// You can pass a combination of one or more styles, for
        /// example `Gx::Text::Bold | Gx::Text::Italic`.
        /// The default style is `Gx::Text::Regular`.
        ///
        /// @param style New style
        ///
        /// @see `GetStyle`
        ////////////////////////////////////////////////////////////
        void SetStyle(std::uint32_t style);

        ////////////////////////////////////////////////////////////
        /// @brief Set the character size
        ///
        /// The default size is 30.
        /// Setting a new size with this method will set character width to 0.
        /// See @a `SetCharacterWidth` for more details.
        ///
        /// Note that if the used font is a bitmap font, it is not
        /// scalable, thus not all requested sizes will be available
        /// to use. This needs to be taken into consideration when
        /// setting the character size. If you need to display text
        /// of a certain size, make sure the corresponding bitmap
        /// font that supports that size is used.
        ///
        /// @param size New character size, in pixels
        ///
        /// @see `GetCharacterSize`
        ////////////////////////////////////////////////////////////
        void SetCharacterSize(unsigned int size);

        ////////////////////////////////////////////////////////////
        /// @brief Set the character width
        ///
        /// The default size is 0, which mean it automatically computed from the character height.
        ///
        /// @param width New character width, in pixels.
        ///              Set it to 0 if you want the font to figure out the width based on the character height.
        ///
        /// @see `GetCharacterWidth`
        ////////////////////////////////////////////////////////////
        void SetCharacterWidth(unsigned int width);

        ////////////////////////////////////////////////////////////
        /// @brief Set the character height.
        ///
        /// The default size is 30.
        ///
        /// Note that if the used font is a bitmap font, it is not
        /// scalable, thus not all requested sizes will be available
        /// to use. This needs to be taken into consideration when
        /// setting the character size. If you need to display text
        /// of a certain size, make sure the corresponding bitmap
        /// font that supports that size is used.
        ///
        /// @param height New character height, in pixels
        ///
        /// @see `GetCharacterHeight`
        ////////////////////////////////////////////////////////////
        void SetCharacterHeight(unsigned int height);

        ////////////////////////////////////////////////////////////
        /// @brief Set the line spacing
        ///
        /// The default spacing between lines is defined by the font.
        /// This method enables you to set a factor for the spacing
        /// between lines. By default, the line spacing is 1.
        ///
        /// @param spacing New line spacing in pixels
        ///
        /// @see `GetLineSpacing`
        ////////////////////////////////////////////////////////////
        void SetLineSpacing(float spacing);

        ////////////////////////////////////////////////////////////
        /// @brief Set the letter spacing
        ///
        /// The default spacing between letters is defined by the font.
        /// This spacing doesn't directly apply to the existing
        /// spacing between each character, it rather adds a fixed
        /// space between them which is calculated from the font
        /// metrics and the character size.
        /// Note that spacing below 1 (including negative numbers) bring
        /// characters closer to each other.
        /// By default, the letter spacing is 1.
        ///
        /// @param spacing New letter spacing in pixels
        ///
        /// @see `GetLetterSpacing`
        ////////////////////////////////////////////////////////////
        void SetLetterSpacing(float spacing);

        ////////////////////////////////////////////////////////////
        /// @brief Set the shear when rendering text in italic
        ///
        /// By default shear is 12.
        ///
        /// @param italicSheer New shear for italic style
        ///
        /// @see `GetItalicShear`
        ////////////////////////////////////////////////////////////
        void SetItalicShear(const sf::Angle& italicSheer);

        ///////////////////////////////////////////////////////////
        /// @brief Set the fill color of the text.
        ///
        /// By default, the text's fill color is opaque white.
        /// Setting the fill color to a transparent color with an outline
        /// will cause the outline to be displayed in the fill area of the text.
        ///
        /// @param color New fill color of the text
        ///
        /// @see `GetColor`
        ////////////////////////////////////////////////////////////
        void SetColor(const sf::Color& color) override;

        ///////////////////////////////////////////////////////////
        /// @brief Set the fill color of a specific character in the text
        ///
        /// By default, the text's fill color is opaque white.
        /// Setting the fill color to a transparent color with an outline
        /// will cause the outline to be displayed in the fill area of the text.
        ///
        /// @param color New fill color of the text
        /// @param index The character index to be colored
        ///
        /// @see `GetColor`
        ////////////////////////////////////////////////////////////
        void SetColor(const sf::Color& color, size_t index);

        ////////////////////////////////////////////////////////////
        /// @brief Set the outline color of the text
        ///
        /// By default, the text's outline color is opaque black.
        ///
        /// @param color New outline color of the text
        ///
        /// @see `GetOutlineColor`
        ////////////////////////////////////////////////////////////
        void SetOutlineColor(const sf::Color& color);

        ////////////////////////////////////////////////////////////
        /// @brief Set the thickness of the text's outline
        ///
        /// By default, the outline thickness is 0.
        ///
        /// Be aware that using a negative value for the outline
        /// thickness will cause distorted rendering.
        ///
        /// @param thickness New outline thickness, in pixels
        ///
        /// @see `GetOutlineThickness`
        ////////////////////////////////////////////////////////////
        void SetOutlineThickness(float thickness);

        ////////////////////////////////////////////////////////////
        /// @brief Set the offset of the text's outline
        ///
        /// By default, the outline offset is 0.
        ///
        /// Be aware that using a negative value for the outline
        /// offset will cause distorted rendering.
        ///
        /// @param offset New outline offset, in pixels
        ///
        /// @see `GetOutlineOffset`
        ////////////////////////////////////////////////////////////
        void SetOutlineOffset(const sf::Vector2f& offset);

        ////////////////////////////////////////////////////////////
        /// @brief Get the text's string
        ///
        /// The returned string is a `sf::String`, which can automatically
        /// be converted to standard string types. So, the following
        /// lines of code are all valid:
        /// @code
        /// sf::String   s1 = text.getString();
        /// std::string  s2 = text.getString();
        /// std::wstring s3 = text.getString();
        /// @endcode
        ///
        /// @return Text's string
        ///
        /// @see `SetString`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::String& GetString() const;
        
        ////////////////////////////////////////////////////////////
        /// @brief Get the text's font
        ///
        /// The returned reference is const, which means that you
        /// cannot modify the font when you get it from this function.
        ///
        /// @return Reference to the text's font
        ///
        /// @see `SetFont`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const Font* GetFont() const;


        ////////////////////////////////////////////////////////////
        /// @brief Get a value determine whether the text is displayed as masked string
        ///
        /// @return `true` if rendered as masked string, otherwise `false`.
        ///
        /// @see `SetMasked`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool IsMasked() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the text's style
        ///
        /// @return Text's style
        ///
        /// @see `setStyle`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] std::uint32_t GetStyle() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the character size
        ///
        /// @return Size of the characters, in pixels
        ///
        /// @see `SetCharacterSize`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] unsigned int GetCharacterSize() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the character width
        ///
        /// @return Width of the characters, in pixels
        ///
        /// @see `SetCharacterWidth`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] unsigned int GetCharacterWidth() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the character height
        ///
        /// @return Height of the characters, in pixels
        ///
        /// @see `SetCharacterHeight`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] unsigned int GetCharacterHeight() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the size of the letter spacing
        ///
        /// @return Size of the letter spacing in pixels
        ///
        /// @see `SetLetterSpacing`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] float GetLetterSpacing() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the size of the line spacing
        ///
        /// @return Size of the line spacing in pixels
        ///
        /// @see `SetLineSpacing`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] float GetLineSpacing() const;
        
        ////////////////////////////////////////////////////////////
        /// @brief Get the shear when rendering text in italic
        ///
        /// @return Shear for italic style
        ///
        /// @see `SetItalicShear`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::Angle GetItalicShear() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the fill color of the text
        ///
        /// @return Fill color of the text
        ///
        /// @see `SetColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetColor() const override;

        ////////////////////////////////////////////////////////////
        /// @brief Get the outline color of the text
        ///
        /// @return Outline color of the text
        ///
        /// @see `SetOutlineColor`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Color& GetOutlineColor() const;
        
        ////////////////////////////////////////////////////////////
        /// @brief Get the outline thickness of the text
        ///
        /// @return Outline thickness of the text, in pixels
        ///
        /// @see `SetOutlineThickness`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] float GetOutlineThickness() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the outline offset of the text
        ///
        /// @return Outline offset of the text, in pixels
        ///
        /// @see `SetOutlineOffset`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::Vector2f GetOutlineOffset() const;

        ////////////////////////////////////////////////////////////
        /// @brief Return the position of the @a `index`-th character
        ///
        /// This function computes the visual position of a character
        /// from its index in the string. The returned position is
        /// in global coordinates (translation, rotation, scale and
        /// origin are applied).
        /// If @a `index` is out of range, the position of the end of
        /// the string is returned.
        ///
        /// @param index Index of the character
        ///
        /// @return Position of the character
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::Vector2f FindCharacterPosition(std::size_t index) const;

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
        [[nodiscard]] virtual sf::FloatRect GetLocalBounds() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the global bounding rectangle of the entity
        ///
        /// The returned rectangle is in global coordinates, which means
        /// that it takes into account the transformations (translation,
        /// rotation, scale, ...) that are applied to the entity.
        /// In other words, this function returns the bounds of the
        /// text in the global 2D world's coordinate system.
        ///
        /// @return Global bounding rectangle of the entity
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::FloatRect GetGlobalBounds() const;

    protected:
        ////////////////////////////////////////////////////////////
        /// @brief Make sure the text's geometry is updated
        ///
        /// All the attributes related to rendering are cached, such
        /// that the geometry is only updated when necessary.
        ////////////////////////////////////////////////////////////
        void EnsureGeometryUpdate() const;

        ////////////////////////////////////////////////////////////
        /// @brief An overridable callback that called when the geometry is updated
        ////////////////////////////////////////////////////////////
        virtual void OnGeometryUpdated() const {};

        ////////////////////////////////////////////////////////////
        /// @brief Render the text to a render surface
        ///
        /// @param surface Render surface to draw to
        /// @param states Current render states
        ////////////////////////////////////////////////////////////
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

    private:
        using ColorMap = std::unordered_map<size_t, sf::Color>;

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        sf::String              m_string;                                        //!< String to display
        const Font*             m_font{};                                        //!< Font used to display the string
        unsigned int            m_characterHeight{30};                           //!< Base height of characters, in pixels
        unsigned int            m_characterWidth{0};                             //!< Base width of characters, in pixels
        float                   m_letterSpacing{0};                              //!< Spacing between letters
        float                   m_lineSpacing{1.f};                              //!< Spacing between lines
        sf::Angle               m_italicShear{sf::degrees(12)};             //!< Shear of italic characters
        std::uint32_t           m_style{Style::Regular};                         //!< Text style (see Style enum)
        sf::Color               m_fillColor{sf::Color::White};                   //!< Text fill color
        sf::Color               m_outlineColor{sf::Color::Black};                //!< Text outline color
        ColorMap                m_colorMap{};                                    //!< Character individual fill colors
        float                   m_outlineThickness{0};                           //!< Thickness of the text's outline
        sf::Vector2f            m_outlineOffset{};                               //!< Offset of the text's outline
        bool                    m_masked{};                                      //!< Does text masking rendering enabled?
        mutable sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};        //!< Vertex array containing the fill geometry
        mutable sf::VertexArray m_outlineVertices{sf::PrimitiveType::Triangles}; //!< Vertex array containing the outline geometry
        mutable sf::FloatRect   m_bounds;                                        //!< Bounding rectangle of the text (in local coordinates)
        mutable bool            m_geometryNeedUpdate{};                          //!< Does the geometry need to be recomputed?
        mutable std::uint64_t   m_fontTextureId{};                               //!< The font texture id
    };

}
