#ifndef GENODE_IO_TEXTURE_LOADER_HPP
#define GENODE_IO_TEXTURE_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Gx
{
    namespace priv
    {
        class TextureLoader : public Gx::ResourceLoader<sf::Texture>
        {
        public:
            virtual bool IsMetadataRequired() const;

            virtual std::unique_ptr<ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
            virtual ResourcePtr<sf::Texture> Load(const ResourceMetadata &metadata, const ResourceContext& context = ResourceContext()) const;
            virtual ResourcePtr<sf::Texture> Load(const void *data, std::size_t size) const;
        };
    }
}

#endif