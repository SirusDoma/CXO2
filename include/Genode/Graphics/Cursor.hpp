#ifndef GENODE_CURSOR_HPP
#define GENODE_CURSOR_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Cursor.hpp>

#include <Genode/System/Primitives.hpp>
#include <vector>

namespace Gx 
{
    class Cursor
    {
    public:
        Cursor();
        Cursor(const sf::Texture& texture);

        sf::Cursor* GetHandle();

    private:
        sf::Cursor*  m_cursor;
        Gx::Uint8*   m_source;
        sf::Vector2u m_size;
    };
}

#endif