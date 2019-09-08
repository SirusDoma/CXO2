#include <Genode/IO/Loaders/TextureLoader.hpp>

namespace Gx
{
    namespace priv
    {
        sf::Texture TextureLoader::Load(Uint8* data, Uint64 size) const
        {
            sf::Texture texture = sf::Texture();
            if (!texture.loadFromMemory(data, static_cast<size_t>(size)))
                return texture;

            texture.setSmooth(true);
            return texture;
        }
    }
}