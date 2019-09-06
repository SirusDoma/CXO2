#ifndef GENODE_FONT_DESERIALIZER_HPP
#define GENODE_FONT_DESERIALIZER_HPP

#include <Genode/IO/ResourceDeserializer.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Gx
{
	namespace priv
	{
		class FontDeserializer : public Gx::ResourceDeserializer<sf::Font>
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