#include <Genode/IO/Loaders/FontLoader.hpp>

namespace Gx
{
	namespace priv
	{
		sf::Font FontLoader::Load(Uint8* data, Uint64 size) const
		{
			sf::Font font = sf::Font();
			font.loadFromMemory(data, static_cast<size_t>(size));

			return font;
		}
	}
}