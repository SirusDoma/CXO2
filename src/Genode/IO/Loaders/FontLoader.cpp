#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/FileHelper.hpp>

namespace Gx
{
    namespace priv
    {
        bool FontLoader::IsMetadataRequired() const
        {
            return false;
        }

        bool FontLoader::IsResourceStream() const
        {
            return true;
        }

        std::unique_ptr<ResourceMetadata> FontLoader::LoadMetadata(const void *data, std::size_t size) const
        {
            return nullptr;
        }

        ResourcePtr<sf::Font> FontLoader::Load(const ResourceMetadata &metadata, const ResourceContext& context) const
        {
            auto required = metadata.GetResourceReference(ResourceReference::Font);
            if (required)
            {
                auto font = std::make_unique<sf::Font>();
                if (font->loadFromFile(required->Value))
                    return font;
            }

            return nullptr;
        }

        ResourcePtr<sf::Font> FontLoader::Load(const void* data, std::size_t size) const
        {
            auto font = std::make_unique<sf::Font>();
            if (!font->loadFromMemory(data, size))
                return nullptr;

            return font;
        }
    }
}