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
			virtual sf::Texture Deserialize(Uint8* data, Uint64 size) const
			{
				sf::Texture texture = sf::Texture();
				if (!texture.loadFromMemory(data, static_cast<size_t>(size)))
					return texture;
				
				texture.setSmooth(true);
				return texture;
			}
		};
	}
}

#endif