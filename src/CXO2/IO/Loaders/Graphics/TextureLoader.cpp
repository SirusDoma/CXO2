#include <CXO2/IO/Loaders/Graphics/TextureLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

namespace Cx
{
    void TextureLoader::UseSmooth(const bool smooth)
    {
        m_smooth = smooth;
    }

    Gx::ResourcePtr<sf::Texture> TextureLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromStream(*stream))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    Gx::ResourcePtr<sf::Texture> TextureLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromMemory(data, size))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    Gx::ResourcePtr<sf::Texture> TextureLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<sf::Texture>();
        if (!resource->loadFromStream(stream))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }
}
