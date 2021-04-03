#include <Genode/IO/Loaders/SoundBufferLoader.hpp>
#include <Genode/IO/FileHelper.hpp>

namespace Gx
{
    namespace priv
    {
        bool SoundBufferLoader::IsMetadataRequired() const
        {
            return false;
        }

        std::unique_ptr<ResourceMetadata> SoundBufferLoader::LoadMetadata(const void *data, std::size_t size) const
        {
            return nullptr;
        }

        ResourcePtr<sf::SoundBuffer> SoundBufferLoader::Load(const ResourceMetadata &metadata, const ResourceContext& context) const
        {
            auto required = metadata.GetResourceReference(ResourceReference::SoundBuffer);
            if (required)
            {
                Gx::Uint8 *data;
                if (auto size = Gx::FileHelper::ReadFile(required->Value, &data))
                {
                    auto soundBuffer = Load(data, size);
                    delete data;

                    return soundBuffer;
                }
            }

            return nullptr;
        }

        ResourcePtr<sf::SoundBuffer> SoundBufferLoader::Load(const void* data, std::size_t size) const
        {
            auto soundBuffer = std::make_unique<sf::SoundBuffer>();
            if (!soundBuffer->loadFromMemory(data, size))
                return nullptr;

            return soundBuffer;
        }
    }
}
