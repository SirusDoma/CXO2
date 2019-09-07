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
			virtual sf::Font Deserialize(Uint8* data, Uint64 size) const
			{
				sf::Font font = sf::Font();
				font.loadFromMemory(data, static_cast<size_t>(size));

				return font;
			}
		};
	}
}

#endif