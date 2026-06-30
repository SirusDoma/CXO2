#include <CXO2/IO/Loaders/Audio/SoundBufferLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

namespace Cx
{
    Gx::ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromStream(*stream))
            return nullptr;

        return resource;
    }

    Gx::ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromMemory(data, size))
            return nullptr;

        return resource;
    }

    Gx::ResourcePtr<sf::SoundBuffer> SoundBufferLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::SoundBuffer>();
        if (!resource->loadFromStream(stream))
            return nullptr;

        return resource;
    }
}
