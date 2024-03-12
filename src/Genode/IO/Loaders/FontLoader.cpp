#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

namespace Gx
{
    bool FontLoader::IsStreaming() const
    {
        return true;
    }

    void FontLoader::UseSmooth(bool smooth)
    {
        m_smooth = smooth;
    }

    ResourcePtr<sf::Font> FontLoader::LoadFromFile(const std::string &fileName, const ResourceContext &ctx) const
    {
        auto resource = std::make_unique<sf::Font>();
        if (!resource->loadFromFile(LocalFileSystem::Instance().GetFullName(fileName)))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<sf::Font> FontLoader::LoadFromMemory(void *data, const std::size_t size, const ResourceContext &ctx) const
    {
        auto resource = std::make_unique<sf::Font>();
        if (!resource->loadFromMemory(data, size))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<sf::Font> FontLoader::LoadFromStream(sf::InputStream &stream, const ResourceContext &ctx) const
    {
        auto resource = std::make_unique<sf::Font>();
        if (!resource->loadFromStream(stream))
            return nullptr;

        resource->setSmooth(m_smooth);
        return resource;
    }
}
