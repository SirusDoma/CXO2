#ifndef GENODE_FONT_LOADER_HPP
#define GENODE_FONT_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Gx
{
    namespace priv
    {
        class FontLoader : public Gx::ResourceLoader<sf::Font>
        {
        public:
            virtual sf::Font Load(Uint8* data, Uint64 size) const;
        };
    }
}

#endif