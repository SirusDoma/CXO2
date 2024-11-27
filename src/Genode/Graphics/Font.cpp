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
#include <Genode/Graphics/Font.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/InputStream.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_SYSTEM_H

#include <ostream>
#include <utility>

#include <cmath>
#include <cstring>

namespace
{
    // FreeType callbacks that operate on a sf::InputStream
    unsigned long read(const FT_Stream rec, const unsigned long offset, unsigned char* buffer, const unsigned long count)
    {
        auto* stream = static_cast<sf::InputStream*>(rec->descriptor.pointer);
        if (stream->seek(offset) == offset)
        {
            if (count > 0)
                return static_cast<unsigned long>(stream->read(reinterpret_cast<char*>(buffer), count).value());

            return 0;
        }

        return count > 0 ? 0 : 1; // error code is 0 if we're reading, or nonzero if we're seeking
    }

    void close(FT_Stream)
    {
    }

    // Helper to interpret memory as a specific type
    template <typename T, typename U>
    inline T reinterpret(const U& input)
    {
        T output;
        std::memcpy(&output, &input, sizeof(U));
        return output;
    }

    // Combine outline thickness, boldness and font glyph index into a single 64-bit key
    std::uint64_t combine(const float outlineThickness, const bool bold, const std::uint32_t index)
    {
        return (static_cast<std::uint64_t>(reinterpret<std::uint32_t>(outlineThickness)) << 32) |
               (static_cast<std::uint64_t>(bold) << 31) | index;
    }

    // Combine characterHeight and characterWidth into a single 64-bit key
    std::uint64_t combine(const std::uint32_t characterWidth, const std::uint32_t characterHeight)
    {
        return static_cast<std::uint64_t>(characterWidth) << 32 |
               static_cast<std::uint64_t>(characterHeight);
    }
}


namespace Gx
{
    ////////////////////////////////////////////////////////////
    struct Font::FontHandles
    {
        FontHandles() = default;

        ~FontHandles()
        {
            // All the function below are safe to call with null pointer arguments.
            // The documentation of FreeType isn't clear on the matter, but the
            // implementation does explicitly check for null.

            FT_Stroker_Done(stroker);
            FT_Done_Face(face);
            // `streamRec` doesn't need to be explicitly freed.
            FT_Done_FreeType(library);
        }

        // clang-format off
        FontHandles(const FontHandles&)            = delete;
        FontHandles& operator=(const FontHandles&) = delete;

        FontHandles(FontHandles&&)            = delete;
        FontHandles& operator=(FontHandles&&) = delete;
        // clang-format on

        FT_Library   library{};   //< Pointer to the internal library interface
        FT_StreamRec streamRec{}; //< Stream rec object describing an input stream
        FT_Face      face{};      //< Pointer to the internal font face
        FT_Stroker   stroker{};   //< Pointer to the stroker
    };


    ////////////////////////////////////////////////////////////
    bool Font::LoadFromFile(const std::string& filename)
    {
        // Get Full name
        const std::string fullName = LocalFileSystem::Instance().GetFullName(filename);

        // Cleanup the previous resources
        Cleanup();

        auto fontHandles = std::make_shared<FontHandles>();

        // Initialize FreeType
        // Note: we initialize FreeType for every font instance in order to avoid having a single
        // global manager that would create a lot of issues regarding creation and destruction order.
        if (FT_Init_FreeType(&fontHandles->library) != 0)
        {
            sf::err() << "Failed to load font (failed to initialize FreeType)\n" << filename << std::endl;
            return false;
        }

        // Load the new font face from the specified file
        FT_Face face = nullptr;
        if (FT_New_Face(fontHandles->library, fullName.c_str(), 0, &face) != 0)
        {
            sf::err() << "Failed to load font (failed to create the font face)\n" << filename << std::endl;
            return false;
        }
        fontHandles->face = face;

        // Load the stroker that will be used to outline the font
        if (FT_Stroker_New(fontHandles->library, &fontHandles->stroker) != 0)
        {
            sf::err() << "Failed to load font (failed to create the stroker)\n" << filename << std::endl;
            return false;
        }

        // Select the unicode character map
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
        {
            sf::err() << "Failed to load font (failed to set the Unicode character set)\n"
                      << filename << std::endl;
            return false;
        }

        // Store the loaded font handles
        m_fontHandles = std::move(fontHandles);

        // Store the font information
        m_info.family = face->family_name ? face->family_name : std::string();

        return true;
    }


    ////////////////////////////////////////////////////////////
    bool Font::LoadFromMemory(const void* data, std::size_t sizeInBytes)
    {
        // Cleanup the previous resources
        Cleanup();

        auto fontHandles = std::make_shared<FontHandles>();

        // Initialize FreeType
        // Note: we initialize FreeType for every font instance in order to avoid having a single
        // global manager that would create a lot of issues regarding creation and destruction order.
        if (FT_Init_FreeType(&fontHandles->library) != 0)
        {
            sf::err() << "Failed to load font from memory (failed to initialize FreeType)" << std::endl;
            return false;
        }

        // Load the new font face from the specified file
        FT_Face face = nullptr;
        if (FT_New_Memory_Face(fontHandles->library,
                               reinterpret_cast<const FT_Byte*>(data),
                               static_cast<FT_Long>(sizeInBytes),
                               0,
                               &face) != 0)
        {
            sf::err() << "Failed to load font from memory (failed to create the font face)" << std::endl;
            return false;
        }
        fontHandles->face = face;

        // Load the stroker that will be used to outline the font
        if (FT_Stroker_New(fontHandles->library, &fontHandles->stroker) != 0)
        {
            sf::err() << "Failed to load font from memory (failed to create the stroker)" << std::endl;
            return false;
        }

        // Select the Unicode character map
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
        {
            sf::err() << "Failed to load font from memory (failed to set the Unicode character set)" << std::endl;
            return false;
        }

        // Store the loaded font handles
        m_fontHandles = std::move(fontHandles);

        // Store the font information
        m_info.family = face->family_name ? face->family_name : std::string();

        return true;
    }


    ////////////////////////////////////////////////////////////
    bool Font::LoadFromStream(sf::InputStream& stream)
    {
        // Cleanup the previous resources
        Cleanup();

        auto fontHandles = std::make_shared<FontHandles>();

        // Initialize FreeType
        // Note: we initialize FreeType for every font instance in order to avoid having a single
        // global manager that would create a lot of issues regarding creation and destruction order.
        if (FT_Init_FreeType(&fontHandles->library) != 0)
        {
            sf::err() << "Failed to load font from stream (failed to initialize FreeType)" << std::endl;
            return false;
        }

        // Make sure that the stream's reading position is at the beginning
        if (!stream.seek(0).has_value())
        {
            sf::err() << "Failed to seek font stream" << std::endl;
            return false;
        }

        // Prepare a wrapper for our stream, that we'll pass to FreeType callbacks
        fontHandles->streamRec.base               = nullptr;
        fontHandles->streamRec.size               = static_cast<unsigned long>(stream.getSize().value());
        fontHandles->streamRec.pos                = 0;
        fontHandles->streamRec.descriptor.pointer = &stream;
        fontHandles->streamRec.read               = &read;
        fontHandles->streamRec.close              = &close;

        // Setup the FreeType callbacks that will read our stream
        FT_Open_Args args;
        args.flags  = FT_OPEN_STREAM;
        args.stream = &fontHandles->streamRec;
        args.driver = nullptr;

        // Load the new font face from the specified stream
        FT_Face face = nullptr;
        if (FT_Open_Face(fontHandles->library, &args, 0, &face) != 0)
        {
            sf::err() << "Failed to load font from stream (failed to create the font face)" << std::endl;
            return false;
        }
        fontHandles->face = face;

        // Load the stroker that will be used to outline the font
        if (FT_Stroker_New(fontHandles->library, &fontHandles->stroker) != 0)
        {
            sf::err() << "Failed to load font from stream (failed to create the stroker)" << std::endl;
            return false;
        }

        // Select the Unicode character map
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
        {
            sf::err() << "Failed to load font from stream (failed to set the Unicode character set)" << std::endl;
            return false;
        }

        // Store the loaded font handles
        m_fontHandles = std::move(fontHandles);

        // Store the font information
        m_info.family = face->family_name ? face->family_name : std::string();

        return true;
    }


    ////////////////////////////////////////////////////////////
    const sf::Font::Info& Font::GetInfo() const
    {
        return m_info;
    }


    ////////////////////////////////////////////////////////////
    const sf::Glyph& Font::GetGlyph(std::uint32_t codePoint, const unsigned int characterWidth, const unsigned int characterHeight, const bool bold, const float outlineThickness) const
    {
        // Get the page corresponding to the character size
        Page& page = LoadPage(characterWidth, characterHeight);
        GlyphTable& glyphs = page.glyphs;

        // Build the key by combining the glyph index (based on code point), bold flag, and outline thickness
        const std::uint64_t key = combine(outlineThickness,
                                          bold,
                                          FT_Get_Char_Index(m_fontHandles ? m_fontHandles->face : nullptr, codePoint));

        // Search the glyph into the cache
        if (const auto it = glyphs.find(key); it != glyphs.end())
        {
            // Found: just return it
            return it->second;
        }
        else
        {
            // Not found: we have to load it
            const sf::Glyph glyph = LoadGlyph(codePoint, characterWidth, characterHeight, bold, outlineThickness);
            return glyphs.emplace(key, glyph).first->second;
        }
    }


    ////////////////////////////////////////////////////////////
    bool Font::HasGlyph(std::uint32_t codePoint) const
    {
        return FT_Get_Char_Index(m_fontHandles ? m_fontHandles->face : nullptr, codePoint) != 0;
    }


    ////////////////////////////////////////////////////////////
    float Font::GetKerning(std::uint32_t first, std::uint32_t second, const unsigned int characterWidth, const unsigned int characterHeight, const bool bold) const
    {
        // Special case where first or second is 0 (null character)
        if (first == 0 || second == 0)
            return 0.f;

        FT_Face face = m_fontHandles ? m_fontHandles->face : nullptr;

        if (face && SetCurrentSize(characterWidth, characterHeight))
        {
            // Convert the characters to indices
            const FT_UInt index1 = FT_Get_Char_Index(face, first);
            const FT_UInt index2 = FT_Get_Char_Index(face, second);

            // Retrieve position compensation deltas generated by FT_LOAD_FORCE_AUTOHINT flag
            const auto firstRsbDelta  = static_cast<float>(GetGlyph(first, characterWidth, characterHeight, bold).rsbDelta);
            const auto secondLsbDelta = static_cast<float>(GetGlyph(second, characterWidth, characterHeight, bold).lsbDelta);

            // Get the kerning vector if present
            FT_Vector kerning{0, 0};
            if (FT_HAS_KERNING(face))
                FT_Get_Kerning(face, index1, index2, FT_KERNING_UNFITTED, &kerning);

            // X advance is already in pixels for bitmap fonts
            if (!FT_IS_SCALABLE(face))
                return static_cast<float>(kerning.x);

            // Combine kerning with compensation deltas and return the X advance
            // Flooring is required as we use FT_KERNING_UNFITTED flag which is not quantized in 64 based grid
            return std::floor(
                (secondLsbDelta - firstRsbDelta + static_cast<float>(kerning.x) + 32) / static_cast<float>(1 << 6));
        }
        else
        {
            // Invalid font
            return 0.f;
        }
    }


    ////////////////////////////////////////////////////////////
    float Font::GetLineSpacing(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        FT_Face face = m_fontHandles ? m_fontHandles->face : nullptr;

        if (face && SetCurrentSize(characterWidth, characterHeight))
        {
            return static_cast<float>(face->size->metrics.height) / static_cast<float>(1 << 6);
        }
        else
        {
            return 0.f;
        }
    }


    ////////////////////////////////////////////////////////////
    float Font::GetUnderlinePosition(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        FT_Face face = m_fontHandles ? m_fontHandles->face : nullptr;

        if (face && SetCurrentSize(characterWidth, characterHeight))
        {
            // Return a fixed position if font is a bitmap font
            if (!FT_IS_SCALABLE(face))
                return static_cast<float>(characterHeight) / 10.f;

            return -static_cast<float>(FT_MulFix(face->underline_position, face->size->metrics.y_scale)) /
                   static_cast<float>(1 << 6);
        }
        else
        {
            return 0.f;
        }
    }


    ////////////////////////////////////////////////////////////
    float Font::GetUnderlineThickness(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        FT_Face face = m_fontHandles ? m_fontHandles->face : nullptr;

        if (face && SetCurrentSize(characterWidth, characterHeight))
        {
            // Return a fixed thickness if font is a bitmap font
            if (!FT_IS_SCALABLE(face))
                return static_cast<float>(characterHeight) / 14.f;

            return static_cast<float>(FT_MulFix(face->underline_thickness, face->size->metrics.y_scale)) /
                   static_cast<float>(1 << 6);
        }
        else
        {
            return 0.f;
        }
    }


    ////////////////////////////////////////////////////////////
    const sf::Texture& Font::GetTexture(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        return LoadPage(characterWidth, characterHeight).texture;
    }

    ////////////////////////////////////////////////////////////
    void Font::SetSmooth(const bool smooth)
    {
        if (smooth != m_isSmooth)
        {
            m_isSmooth = smooth;

            for (auto& [key, page] : m_pages)
            {
                page.texture.setSmooth(m_isSmooth);
            }
        }
    }

    ////////////////////////////////////////////////////////////
    bool Font::IsSmooth() const
    {
        return m_isSmooth;
    }


    ////////////////////////////////////////////////////////////
    void Font::Cleanup()
    {
        // Drop ownership of shared FreeType pointers
        m_fontHandles.reset();

        // Reset members
        m_pages.clear();
        std::vector<std::uint8_t>().swap(m_pixelBuffer);
    }


    ////////////////////////////////////////////////////////////
    Font::Page& Font::LoadPage(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        return m_pages.try_emplace(combine(characterWidth, characterHeight), m_isSmooth).first->second;
    }


    ////////////////////////////////////////////////////////////
    sf::Glyph Font::LoadGlyph(std::uint32_t codePoint, const unsigned int characterWidth, const unsigned int characterHeight, const bool bold, const float outlineThickness) const
    {
        // The glyph to return
        sf::Glyph glyph;

        // Stop if no font is loaded
        if (!m_fontHandles)
            return glyph;

        // Get our FT_Face
        FT_Face face = m_fontHandles->face;
        if (!face)
            return glyph;

        // Set the character size
        if (!SetCurrentSize(characterWidth, characterHeight))
            return glyph;

        // Load the glyph corresponding to the code point
        FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
        if (outlineThickness != 0)
            flags |= FT_LOAD_NO_BITMAP;
        if (FT_Load_Char(face, codePoint, flags) != 0)
            return glyph;

        // Retrieve the glyph
        FT_Glyph glyphDesc;
        if (FT_Get_Glyph(face->glyph, &glyphDesc) != 0)
            return glyph;

        // Apply bold and outline (there is no fallback for outline) if necessary -- first technique using outline (highest quality)
        const FT_Pos weight  = 1 << 6;
        const bool   outline = (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE);
        if (outline)
        {
            if (bold)
            {
                auto* outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyphDesc);
                FT_Outline_Embolden(&outlineGlyph->outline, weight);
            }

            if (outlineThickness != 0)
            {
                FT_Stroker stroker = m_fontHandles->stroker;

                FT_Stroker_Set(stroker,
                               static_cast<FT_Fixed>(outlineThickness * static_cast<float>(1 << 6)),
                               FT_STROKER_LINECAP_ROUND,
                               FT_STROKER_LINEJOIN_ROUND,
                               0);
                FT_Glyph_Stroke(&glyphDesc, stroker, true);
            }
        }

        // Convert the glyph to a bitmap (i.e. rasterize it)
        // Warning! After this line, do not read any data from glyphDesc directly, use
        // bitmapGlyph.root to access the FT_Glyph data.
        FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, nullptr, 1);
        auto*      bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyphDesc);
        FT_Bitmap& bitmap      = bitmapGlyph->bitmap;

        // Apply bold if necessary -- fallback technique using bitmap (lower quality)
        if (!outline)
        {
            if (bold)
                FT_Bitmap_Embolden(m_fontHandles->library, &bitmap, weight, weight);

            if (outlineThickness != 0)
                sf::err() << "Failed to outline glyph (no fallback available)" << std::endl;
        }

        // Compute the glyph's advance offset
        glyph.advance = static_cast<float>(bitmapGlyph->root.advance.x >> 16);
        if (bold)
            glyph.advance += static_cast<float>(weight) / static_cast<float>(1 << 6);

        glyph.lsbDelta = static_cast<int>(face->glyph->lsb_delta);
        glyph.rsbDelta = static_cast<int>(face->glyph->rsb_delta);

        unsigned int width  = bitmap.width;
        unsigned int height = bitmap.rows;

        if ((width > 0) && (height > 0))
        {
            // Leave a small padding around characters, so that filtering doesn't
            // pollute them with pixels from neighbors
            const unsigned int padding = 2;

            width += 2 * padding;
            height += 2 * padding;

            // Get the glyphs page corresponding to the character size
            Page& page = LoadPage(characterWidth, characterHeight);

            // Find a good position for the new glyph into the texture
            glyph.textureRect = FindGlyphRect(page, {width, height});

            // Make sure the texture data is positioned in the center
            // of the allocated texture rectangle
            glyph.textureRect.position.x += static_cast<int>(padding);
            glyph.textureRect.position.y += static_cast<int>(padding);
            glyph.textureRect.size.x -= static_cast<int>(2 * padding);
            glyph.textureRect.size.y -= static_cast<int>(2 * padding);

            // Compute the glyph's bounding box
            glyph.bounds.position.x = static_cast<float>(bitmapGlyph->left);
            glyph.bounds.position.y = static_cast<float>(-bitmapGlyph->top);
            glyph.bounds.size.x = static_cast<float>(bitmap.width);
            glyph.bounds.size.y = static_cast<float>(bitmap.rows);

            // Resize the pixel buffer to the new size and fill it with transparent white pixels
            m_pixelBuffer.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4);

            std::uint8_t* current = m_pixelBuffer.data();
            std::uint8_t* end     = current + width * height * 4;

            while (current != end)
            {
                (*current++) = 255;
                (*current++) = 255;
                (*current++) = 255;
                (*current++) = 0;
            }

            // Extract the glyph's pixels from the bitmap
            const std::uint8_t* pixels = bitmap.buffer;
            if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
            {
                // Pixels are 1 bit monochrome values
                for (unsigned int y = padding; y < height - padding; ++y)
                {
                    for (unsigned int x = padding; x < width - padding; ++x)
                    {
                        // The color channels remain white, just fill the alpha channel
                        const std::size_t index = x + y * width;
                        m_pixelBuffer[index * 4 + 3] = ((pixels[(x - padding) / 8]) & (1 << (7 - ((x - padding) % 8)))) ? 255 : 0;
                    }
                    pixels += bitmap.pitch;
                }
            }
            else
            {
                // Pixels are 8 bits gray levels
                for (unsigned int y = padding; y < height - padding; ++y)
                {
                    for (unsigned int x = padding; x < width - padding; ++x)
                    {
                        // The color channels remain white, just fill the alpha channel
                        const std::size_t index      = x + y * width;
                        m_pixelBuffer[index * 4 + 3] = pixels[x - padding];
                    }
                    pixels += bitmap.pitch;
                }
            }

            // Write the pixels to the texture
            const unsigned int x = static_cast<unsigned int>(glyph.textureRect.position.x) - padding;
            const unsigned int y = static_cast<unsigned int>(glyph.textureRect.position.y) - padding;
            const unsigned int w = static_cast<unsigned int>(glyph.textureRect.size.x) + 2 * padding;
            const unsigned int h = static_cast<unsigned int>(glyph.textureRect.size.y) + 2 * padding;
            page.texture.update(m_pixelBuffer.data(), {w, h}, {x, y});
        }

        // Delete the FT glyph
        FT_Done_Glyph(glyphDesc);

        // Done :)
        return glyph;
    }


    ////////////////////////////////////////////////////////////
    sf::IntRect Font::FindGlyphRect(Page& page, const sf::Vector2u& size) const
    {
        // Find the line that fits well the glyph
        Row*  row       = nullptr;
        float bestRatio = 0;
        for (auto it = page.rows.begin(); it != page.rows.end() && !row; ++it)
        {
            const float ratio = static_cast<float>(size.y) / static_cast<float>(it->height);

            // Ignore rows that are either too small or too high
            if ((ratio < 0.7f) || (ratio > 1.f))
                continue;

            // Check if there's enough horizontal space left in the row
            if (size.x > page.texture.getSize().x - it->width)
                continue;

            // Make sure that this new row is the best found so far
            if (ratio < bestRatio)
                continue;

            // The current row passed all the tests: we can select it
            row       = &*it;
            bestRatio = ratio;
        }

        // If we didn't find a matching row, create a new one (10% taller than the glyph)
        if (!row)
        {
            const unsigned int rowHeight = size.y + size.y / 10;
            while ((page.nextRow + rowHeight >= page.texture.getSize().y) || (size.x >= page.texture.getSize().x))
            {
                // Not enough space: resize the texture if possible
                const sf::Vector2u textureSize = page.texture.getSize();
                if ((textureSize.x * 2 <= sf::Texture::getMaximumSize()) && (textureSize.y * 2 <= sf::Texture::getMaximumSize()))
                {
                    // Make the texture 2 times bigger
                    sf::Texture newTexture(textureSize * 2u);
                    newTexture.setSmooth(m_isSmooth);
                    newTexture.update(page.texture);
                    page.texture.swap(newTexture);
                }
                else
                {
                    // Oops, we've reached the maximum texture size...
                    sf::err() << "Failed to add a new character to the font: the maximum texture size has been reached"
                          << std::endl;
                    return {{0, 0}, {2, 2}};
                }
            }

            // We can now create the new row
            page.rows.emplace_back(page.nextRow, rowHeight);
            page.nextRow += rowHeight;
            row = &page.rows.back();
        }

        // Find the glyph's rectangle on the selected row
        sf::IntRect rect(sf::Rect<unsigned int>({row->width, row->top}, size));

        // Update the row information
        row->width += size.x;

        return rect;
    }


    ////////////////////////////////////////////////////////////
    bool Font::SetCurrentSize(const unsigned int characterWidth, const unsigned int characterHeight) const
    {
        // FT_Set_Pixel_Sizes is an expensive function, so we must call it
        // only when necessary to avoid killing performances

        // m_fontHandles and m_fontHandles->face are checked to be non-null before calling this method
        FT_Face         face          = m_fontHandles->face;
        const FT_UShort currentWidth  = face->size->metrics.x_ppem;
        const FT_UShort currentHeight = face->size->metrics.y_ppem;

        if ((characterWidth != 0 && characterWidth != currentWidth) || characterHeight != currentHeight)
        {
            const FT_Error result = FT_Set_Pixel_Sizes(face, characterWidth, characterHeight);

            if (result == FT_Err_Invalid_Pixel_Size)
            {
                // In the case of bitmap fonts, resizing can
                // fail if the requested size is not available
                if (!FT_IS_SCALABLE(face))
                {
                    sf::err() << "Failed to set bitmap font size to " << characterHeight << '\n' << "Available sizes are: ";
                    for (unsigned int i = 0; i < face->num_fixed_sizes; ++i)
                    {
                        const long size = (face->available_sizes[i].y_ppem + 32) >> 6;
                        sf::err() << size << ' ';
                    }
                    sf::err() << std::endl;
                }
                else
                {
                    sf::err() << "Failed to set font size to " << characterHeight << std::endl;
                }
            }

            return result == FT_Err_Ok;
        }

        return true;
    }


    ////////////////////////////////////////////////////////////
    Font::Page::Page(const bool smooth)
    {
        // Make sure that the texture is initialized by default
        sf::Image image({128, 128}, sf::Color::Transparent);

        // Reserve a 2x2 white square for texturing underlines
        for (unsigned int x = 0; x < 2; ++x)
            for (unsigned int y = 0; y < 2; ++y)
                image.setPixel({x, y}, sf::Color::White);

        // Create the texture
        if (!texture.loadFromImage(image))
        {
            sf::err() << "Failed to load font page texture" << std::endl;
        }

        texture.setSmooth(smooth);
    }

}
