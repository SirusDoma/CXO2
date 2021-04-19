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

#ifndef GENODE_GRAPHICS_TEXT_HPP
#define GENODE_GRAPHICS_TEXT_HPP

#include <Genode/SceneGraph.hpp>
#include <Genode/Entities.hpp>
#include <Genode/System/Primitives.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace
{
    struct CacheStorage {
        typedef sf::Uint64 sf::Texture::* type;
        friend type Get(CacheStorage);
    };

    template<typename Tag, typename Tag::type M>
    struct Hack {
        friend typename Tag::type Get(Tag) {
            return M;
        }
    };

    template struct Hack<CacheStorage, & sf::Texture::m_cacheId>;
}

namespace Gx
{
    typedef std::shared_ptr<const sf::Font> FontHandle;

    class Text : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer, public Colorable
    {
    public:
        enum Style
        {
            Regular       = 0,
            Bold          = 1 << 0,
            Italic        = 1 << 1,
            Underlined    = 1 << 2, 
            StrikeThrough = 1 << 3  
        };

        Text();
        Text(const sf::String& string, const sf::Font& font, unsigned int characterSize = 30);

        void SetString(const sf::String& string);
        void SetFont(const sf::Font& font);

        void SetCharacterSize(unsigned int size);
        void SetLineSpacing(float spacingFactor);
        void SetLetterSpacing(float spacingFactor);
        void SetStyle(Uint32 style);

        virtual void SetColor(const sf::Color& color);
        void SetFillColor(const sf::Color& color);
        void SetFillColor(const sf::Color& color, size_t index);
        void SetOutlineColor(const sf::Color& color);
        void SetOutlineThickness(float thickness);

        const sf::String& GetString() const;
        const sf::Font* GetFont() const;

        unsigned int GetCharacterSize() const;
        float GetLetterSpacing() const;
        float GetLineSpacing() const;
        Uint32 GetStyle() const;

        virtual const sf::Color& GetColor() const;
        const sf::Color& GetFillColor() const;
        const sf::Color& GetOutlineColor() const;
        float GetOutlineThickness() const;

        sf::Vector2f FindCharacterPosition(std::size_t index) const;
        sf::FloatRect GetLocalBounds() const;
        sf::FloatRect GetGlobalBounds() const;

    protected:
        void EnsureGeometryUpdate() const;
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        using ColorMap = std::unordered_map<size_t, sf::Color>;

        sf::String             m_string;
        const sf::Font*         m_font;
        unsigned int            m_characterSize;
        float                   m_letterSpacingFactor;
        float                   m_lineSpacingFactor;
        Uint32                  m_style;
        sf::Color               m_fillColor;
        sf::Color               m_outlineColor;
        float                   m_outlineThickness;
        mutable sf::VertexArray m_vertices;
        mutable sf::VertexArray m_outlineVertices;
        mutable sf::FloatRect   m_bounds;
        mutable bool            m_geometryNeedUpdate; 
        mutable Uint64          m_fontTextureId;
        mutable ColorMap        m_colorMap;
    };

}

#endif