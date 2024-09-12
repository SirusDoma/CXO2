#include <Genode/IO/Loaders/SoundBufferLoader.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

namespace Gx
{
    ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromFile(LocalFileSystem::Instance().GetFullName(fileName)))
            return nullptr;

        return resource;
    }

    ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromMemory(void* data, const std::size_t size, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromMemory(data, size))
            return nullptr;

        return resource;
    }

    ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromStream(stream))
            return nullptr;

        return resource;
    }
}
