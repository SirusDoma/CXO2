#ifndef GENODE_TEXTURE_LOADER_HPP
#define GENODE_TEXTURE_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Gx
{
	namespace priv
	{
		class TextureLoader : public Gx::ResourceLoader<sf::Texture>
		{
		public:
			virtual sf::Texture Load(Uint8* data, Uint64 size) const;
		};
	}
}

#endif