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
            bool IsMetadataRequired() const;

            virtual std::unique_ptr<ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
            virtual ResourcePtr<sf::SoundBuffer> Load(const ResourceMetadata &metadata, const ResourceContext& context = ResourceContext()) const;
            virtual ResourcePtr<sf::SoundBuffer> Load(const void *data, std::size_t size) const;
        };
    }
}

#endif