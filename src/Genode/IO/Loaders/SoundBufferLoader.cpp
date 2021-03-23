#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

namespace Gx
{
    namespace priv
    {
        sf::SoundBuffer SoundBufferLoader::Load(Uint8* data, Uint64 size) const
        {
            sf::SoundBuffer soundBuffer = sf::SoundBuffer();
            soundBuffer.loadFromMemory(data, static_cast<size_t>(size));

            return soundBuffer;
        }
    }
}