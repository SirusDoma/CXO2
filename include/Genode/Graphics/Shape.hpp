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

#ifndef GENODE_SHAPE_HPP
#define GENODE_SHAPE_HPP

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph/Node.hpp>

namespace Gx
{
    class Shape : public Node, public Renderable, public Updatable, public Inputable, public Colorable
    {
    public:
        virtual ~Shape();

        void SetTexture(const sf::Texture* texture, bool resetRect = false);
        void SetTexCoords(const sf::IntRect& rect);

        void SetFillColor(const sf::Color& color);
        void SetOutlineColor(const sf::Color& color);
        void SetOutlineThickness(float thickness);

        const sf::Texture* GetTexture() const;
        const sf::IntRect& GetTexCoords() const;

        const sf::Color& GetFillColor() const;
        const sf::Color& GetOutlineColor() const;
        float GetOutlineThickness() const;
        
        virtual const sf::Color& GetColor() const;
        virtual void SetColor(const sf::Color& color);

        virtual std::size_t GetPointCount() const = 0;
        virtual sf::Vector2f GetPoint(std::size_t index) const = 0;

        sf::FloatRect GetLocalBounds() const;
        sf::FloatRect GetGlobalBounds() const;

    protected:
        Shape();
        void Update();

        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

    private:
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

        void UpdateFillColors();
        void UpdateTexCoords();
        void UpdateOutline();
        void UpdateOutlineColors();

    private:
        const sf::Texture* m_texture;          
        sf::IntRect        m_textureRect;      
        sf::Color          m_fillColor;        
        sf::Color          m_outlineColor;     
        float              m_outlineThickness; 
        sf::VertexArray    m_vertices;         
        sf::VertexArray    m_outlineVertices;  
        sf::FloatRect      m_insideBounds;     
        sf::FloatRect      m_bounds;           
    };

}

#endif
