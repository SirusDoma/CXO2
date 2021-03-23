#ifndef GENODE_SOUND_BUFFER_LOADER_HPP
#define GENODE_SOUND_BUFFER_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Gx
{
    namespace priv
    {
        class SoundBufferLoader : public Gx::ResourceLoader<sf::SoundBuffer>
        {
        public:
            virtual sf::SoundBuffer Load(Uint8* data, Uint64 size) const;
        };
    }
}

#endif