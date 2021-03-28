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
            virtual bool IsMetadataRequired() const;
            virtual bool IsResourceStream() const;

            virtual std::unique_ptr<ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
            virtual ResourcePtr<sf::Font> Load(const ResourceMetadata &metadata, const ResourceContext& context = ResourceContext()) const;
            virtual ResourcePtr<sf::Font> Load(const void* data, std::size_t size) const;
        };
    }
}

#endif