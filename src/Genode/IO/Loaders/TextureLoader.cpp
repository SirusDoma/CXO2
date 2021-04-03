#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/FileHelper.hpp>

namespace Gx
{
    namespace priv
    {
        bool TextureLoader::IsMetadataRequired() const
        {
            return false;
        }

        std::unique_ptr<ResourceMetadata> TextureLoader::LoadMetadata(const void *data, std::size_t size) const
        {
            return nullptr;
        }

        ResourcePtr<sf::Texture> TextureLoader::Load(const ResourceMetadata &metadata, const ResourceContext& context) const
        {
            auto required = metadata.GetResourceReference(ResourceReference::Texture);
            if (required)
            {
                Gx::Uint8 *data;
                if (auto size = Gx::FileHelper::ReadFile(required->Value, &data))
                {
                    auto texture = Load(data, size);
                    delete data;

                    return texture;
                }
            }

            return nullptr;
        }

        ResourcePtr<sf::Texture> TextureLoader::Load(const void *data, std::size_t size) const
        {
            auto texture = std::make_unique<sf::Texture>();
            if (!texture->loadFromMemory(data, size))
                return nullptr;

            texture->setSmooth(true);
            return texture;
        }
    }
}
