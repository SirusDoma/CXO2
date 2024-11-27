#pragma once

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
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/InputStream.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace Gx
{
    ////////////////////////////////////////////////////////////
    /// \brief Class for loading and manipulating character fonts
    ///
    ////////////////////////////////////////////////////////////
    class Font
    {
    public:
        ////////////////////////////////////////////////////////////
        /// \brief Load the font from a file
        ///
        /// The supported font formats are: TrueType, Type 1, CFF,
        /// OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
        /// Note that this function knows nothing about the standard
        /// fonts installed on the user's system, thus you can't
        /// load them directly.
        ///
        /// \warning SFML cannot preload all the font data in this
        /// function, so the file has to remain accessible until
        /// the sf::Font object loads a new font or is destroyed.
        ///
        /// \param filename Path of the font file to load
        ///
        /// \return True if loading succeeded, false if it failed
        ///
        /// \see LoadFromMemory, LoadFromStream
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool LoadFromFile(const std::string& filename);

        ////////////////////////////////////////////////////////////
        /// \brief Load the font from a file in memory
        ///
        /// The supported font formats are: TrueType, Type 1, CFF,
        /// OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
        ///
        /// \warning SFML cannot preload all the font data in this
        /// function, so the buffer pointed by \a data has to remain
        /// valid until the sf::Font object loads a new font or
        /// is destroyed.
        ///
        /// \param data        Pointer to the file data in memory
        /// \param sizeInBytes Size of the data to load, in bytes
        ///
        /// \return True if loading succeeded, false if it failed
        ///
        /// \see LoadFromFile, LoadFromStream
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool LoadFromMemory(const void* data, std::size_t sizeInBytes);

        ////////////////////////////////////////////////////////////
        /// \brief Load the font from a custom stream
        ///
        /// The supported font formats are: TrueType, Type 1, CFF,
        /// OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
        /// Warning: SFML cannot preload all the font data in this
        /// function, so the contents of \a stream have to remain
        /// valid as long as the font is used.
        ///
        /// \warning SFML cannot preload all the font data in this
        /// function, so the stream has to remain accessible until
        /// the sf::Font object loads a new font or is destroyed.
        ///
        /// \param stream Source stream to read from
        ///
        /// \return True if loading succeeded, false if it failed
        ///
        /// \see LoadFromFile, LoadFromMemory
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool LoadFromStream(sf::InputStream& stream);

        ////////////////////////////////////////////////////////////
        /// \brief Get the font information
        ///
        /// \return A structure that holds the font information
        ///
        ////////////////////////////////////////////////////////////
        const sf::Font::Info& GetInfo() const;

        ////////////////////////////////////////////////////////////
        /// \brief Retrieve a glyph of the font
        ///
        /// If the font is a bitmap font, not all character sizes
        /// might be available. If the glyph is not available at the
        /// requested size, an empty glyph is returned.
        ///
        /// You may want to use \ref hasGlyph to determine if the
        /// glyph exists before requesting it. If the glyph does not
        /// exist, a font specific default is returned.
        ///
        /// Be aware that using a negative value for the outline
        /// thickness will cause distorted rendering.
        ///
        /// \param codePoint        Unicode code point of the character to get
        /// \param characterWidth   Reference character width
        /// \param characterHeight  Reference character height
        /// \param bold             Retrieve the bold version or the regular one?
        /// \param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)
        ///
        /// \return The glyph corresponding to \a codePoint and \a characterSize
        ///
        ////////////////////////////////////////////////////////////
        const sf::Glyph& GetGlyph(std::uint32_t codePoint, unsigned int characterWidth, unsigned int characterHeight, bool bold, float outlineThickness = 0) const;

        ////////////////////////////////////////////////////////////
        /// \brief Determine if this font has a glyph representing the requested code point
        ///
        /// Most fonts only include a very limited selection of glyphs from
        /// specific Unicode subsets, like Latin, Cyrillic, or Asian characters.
        ///
        /// While code points without representation will return a font specific
        /// default character, it might be useful to verify whether specific
        /// code points are included to determine whether a font is suited
        /// to display text in a specific language.
        ///
        /// \param codePoint Unicode code point to check
        ///
        /// \return True if the codepoint has a glyph representation, false otherwise
        ///
        ////////////////////////////////////////////////////////////
        bool HasGlyph(std::uint32_t codePoint) const;

        ////////////////////////////////////////////////////////////
        /// \brief Get the kerning offset of two glyphs
        ///
        /// The kerning is an extra offset (negative) to apply between two
        /// glyphs when rendering them, to make the pair look more "natural".
        /// For example, the pair "AV" have a special kerning to make them
        /// closer than other characters. Most of the glyphs pairs have a
        /// kerning offset of zero, though.
        ///
        /// \param first           Unicode code point of the first character
        /// \param second          Unicode code point of the second character
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        /// \param bold            Retrieve the bold version or the regular one?
        ///
        /// \return Kerning value for \a first and \a second, in pixels
        ///
        ////////////////////////////////////////////////////////////
        float GetKerning(std::uint32_t first, std::uint32_t second, unsigned int characterWidth, unsigned int characterHeight, bool bold = false) const;

        ////////////////////////////////////////////////////////////
        /// \brief Get the line spacing
        ///
        /// Line spacing is the vertical offset to apply between two
        /// consecutive lines of text.
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return Line spacing, in pixels
        ///
        ////////////////////////////////////////////////////////////
        float GetLineSpacing(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        /// \brief Get the position of the underline
        ///
        /// Underline position is the vertical offset to apply between the
        /// baseline and the underline.
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return Underline position, in pixels
        ///
        /// \see GetUnderlineThickness
        ///
        ////////////////////////////////////////////////////////////
        float GetUnderlinePosition(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        /// \brief Get the thickness of the underline
        ///
        /// Underline thickness is the vertical size of the underline.
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return Underline thickness, in pixels
        ///
        /// \see GetUnderlinePosition
        ///
        ////////////////////////////////////////////////////////////
        float GetUnderlineThickness(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        /// \brief Retrieve the texture containing the loaded glyphs of a certain size
        ///
        /// The contents of the returned texture changes as more glyphs
        /// are requested, thus it is not very relevant. It is mainly
        /// used internally by sf::Text.
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return Texture containing the glyphs of the requested size
        ///
        ////////////////////////////////////////////////////////////
        const sf::Texture& GetTexture(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        /// \brief Enable or disable the smooth filter
        ///
        /// When the filter is activated, the font appears smoother
        /// so that pixels are less noticeable. However if you want
        /// the font to look exactly the same as its source file,
        /// you should disable it.
        /// The smooth filter is enabled by default.
        ///
        /// \param smooth True to enable smoothing, false to disable it
        ///
        /// \see IsSmooth
        ///
        ////////////////////////////////////////////////////////////
        void SetSmooth(bool smooth);

        ////////////////////////////////////////////////////////////
        /// \brief Tell whether the smooth filter is enabled or not
        ///
        /// \return True if smoothing is enabled, false if it is disabled
        ///
        /// \see SetSmooth
        ///
        ////////////////////////////////////////////////////////////
        bool IsSmooth() const;

    private:
        ////////////////////////////////////////////////////////////
        /// \brief Structure defining a row of glyphs
        ///
        ////////////////////////////////////////////////////////////
        struct Row
        {
            Row(unsigned int rowTop, unsigned int rowHeight) : top(rowTop), height(rowHeight) {}

            unsigned int width{}; //!< Current width of the row
            unsigned int top;     //!< Y position of the row into the texture
            unsigned int height;  //!< Height of the row
        };

        ////////////////////////////////////////////////////////////
        // Types
        ////////////////////////////////////////////////////////////
        using GlyphTable = std::unordered_map<std::uint64_t, sf::Glyph>; //!< Table mapping a codepoint to its glyph

        ////////////////////////////////////////////////////////////
        /// \brief Structure defining a page of glyphs
        ///
        ////////////////////////////////////////////////////////////
        struct Page
        {
            explicit Page(bool smooth);

            GlyphTable       glyphs;     //!< Table mapping code points to their corresponding glyph
            sf::Texture      texture;    //!< Texture containing the pixels of the glyphs
            unsigned int     nextRow{3}; //!< Y position of the next new row in the texture
            std::vector<Row> rows;       //!< List containing the position of all the existing rows
        };

        ////////////////////////////////////////////////////////////
        /// \brief Free all the internal resources
        ///
        ////////////////////////////////////////////////////////////
        void Cleanup();

        ////////////////////////////////////////////////////////////
        /// \brief Find or create the glyphs page corresponding to the given character size
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return The glyphs page corresponding to \a characterHeight and \a characterWidth
        ///
        ////////////////////////////////////////////////////////////
        Page& LoadPage(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        /// \brief Load a new glyph and store it in the cache
        ///
        /// \param codePoint        Unicode code point of the character to load
        /// \param characterWidth   Reference character width
        /// \param characterHeight  Reference character height
        /// \param bold             Retrieve the bold version or the regular one?
        /// \param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)
        ///
        /// \return The glyph corresponding to \a codePoint and \a characterSize
        ///
        ////////////////////////////////////////////////////////////
        sf::Glyph LoadGlyph(std::uint32_t codePoint, unsigned int characterWidth, unsigned int characterHeight, bool bold, float outlineThickness) const;

        ////////////////////////////////////////////////////////////
        /// \brief Find a suitable rectangle within the texture for a glyph
        ///
        /// \param page Page of glyphs to search in
        /// \param size Width and height of the rectangle
        ///
        /// \return Found rectangle within the texture
        ///
        ////////////////////////////////////////////////////////////
        sf::IntRect FindGlyphRect(Page& page, const sf::Vector2u& size) const;

        ////////////////////////////////////////////////////////////
        /// \brief Make sure that the given size is the current one
        ///
        /// \param characterWidth  Reference character width
        /// \param characterHeight Reference character height
        ///
        /// \return True on success, false if any error happened
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool SetCurrentSize(unsigned int characterWidth, unsigned int characterHeight) const;

        ////////////////////////////////////////////////////////////
        // Types
        ////////////////////////////////////////////////////////////
        struct FontHandles;
        using PageTable = std::unordered_map<std::uint64_t, Page>; //!< Table mapping a character size to its page (texture)

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        std::shared_ptr<FontHandles>      m_fontHandles;    //!< Shared information about the internal font instance
        bool                              m_isSmooth{true}; //!< Status of the smooth filter
        sf::Font::Info                    m_info;           //!< Information about the font
        mutable PageTable                 m_pages;          //!< Table containing the glyphs pages by character size
        mutable std::vector<std::uint8_t> m_pixelBuffer;    //!< Pixel buffer holding a glyph's pixels before being written to the texture
    };

}

////////////////////////////////////////////////////////////
/// \class sf::Font
/// \ingroup graphics
///
/// Fonts can be loaded from a file, from memory or from a custom
/// stream, and supports the most common types of fonts. See
/// the loadFromFile function for the complete list of supported formats.
///
/// Once it is loaded, a sf::Font instance provides three
/// types of information about the font:
/// \li Global metrics, such as the line spacing
/// \li Per-glyph metrics, such as bounding box or kerning
/// \li Pixel representation of glyphs
///
/// Fonts alone are not very useful: they hold the font data
/// but cannot make anything useful of it. To do so you need to
/// use the sf::Text class, which is able to properly output text
/// with several options such as character size, style, color,
/// position, rotation, etc.
/// This separation allows more flexibility and better performances:
/// indeed a sf::Font is a heavy resource, and any operation on it
/// is slow (often too slow for real-time applications). On the other
/// side, a sf::Text is a lightweight object which can combine the
/// glyphs data and metrics of a sf::Font to display any text on a
/// render target.
/// Note that it is also possible to bind several sf::Text instances
/// to the same sf::Font.
///
/// It is important to note that the sf::Text instance doesn't
/// copy the font that it uses, it only keeps a reference to it.
/// Thus, a sf::Font must not be destructed while it is
/// used by a sf::Text (i.e. never write a function that
/// uses a local sf::Font instance for creating a text).
///
/// Usage example:
/// \code
/// // Declare a new font
/// sf::Font font;
///
/// // Load it from a file
/// if (!font.loadFromFile("arial.ttf"))
/// {
///     // error...
/// }
///
/// // Create a text which uses our font
/// sf::Text text1(font);
/// text1.setCharacterSize(30);
/// text1.setStyle(sf::Text::Regular);
///
/// // Create another text using the same font, but with different parameters
/// sf::Text text2(font);
/// text2.setCharacterSize(50);
/// text2.setStyle(sf::Text::Italic);
/// \endcode
///
/// Apart from loading font files, and passing them to instances
/// of sf::Text, you should normally not have to deal directly
/// with this class. However, it may be useful to access the
/// font metrics or rasterized glyphs for advanced usage.
///
/// Note that if the font is a bitmap font, it is not scalable,
/// thus not all requested sizes will be available to use. This
/// needs to be taken into consideration when using sf::Text.
/// If you need to display text of a certain size, make sure the
/// corresponding bitmap font that supports that size is used.
///
/// \see sf::Text
///
////////////////////////////////////////////////////////////
