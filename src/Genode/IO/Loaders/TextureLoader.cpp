#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>


namespace Gx
{
    void TextureLoader::UseSmooth(const bool smooth)
    {
        m_smooth = smooth;
    }

    ResourcePtr<sf::Texture> TextureLoader::LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromFile(LocalFileSystem::Instance().GetFullName(fileName)))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<sf::Texture> TextureLoader::LoadFromMemory(void* data, const std::size_t size, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromMemory(data, size))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<sf::Texture> TextureLoader::LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromStream(stream))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }
}
