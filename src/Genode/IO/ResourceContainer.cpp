#include <Genode/IO/ResourceContainer.hpp>

namespace Gx
{
    ResourceContainer::ResourceContainer() :
        m_caches(),
        m_resources(),
        m_fileSystem()
    {
        SetFileSystem(FileSystem::Instance());
    }

    ResourceContainer::~ResourceContainer()
    {
        for (auto archive : m_archives)
        {
            if (archive.second)
                delete archive.second;
        }

        // Clearing cache will kill weakptr and sharedptr will be killed on the last instance
        m_caches.clear();
        m_resources.clear();
        m_archives.clear();
    }

    ResourceContainer* ResourceContainer::Instance()
    {
        static ResourceContainer instance = ResourceContainer();
        return &instance;
    }

    const FileSystem* ResourceContainer::GetFileSystem() const
    {
        return m_fileSystem;
    }

    void ResourceContainer::SetFileSystem(const FileSystem* system)
    {
        m_fileSystem = system;
    }

    bool ResourceContainer::Remove(const std::string& identifier)
    {
        Uncache(identifier);

        auto iterator = m_resources.find(identifier);
        if (iterator != m_resources.end())
        {
            m_resources.erase(iterator);
            return true;
        }

        return false;
    }

    TexturePtr ResourceContainer::LoadTexture(const std::string& identifier, Uint8* data, Int64 size)
    {
        m_resources[identifier] = std::make_shared<sf::Texture>();
        auto texture = std::get<std::shared_ptr<sf::Texture>>(m_resources[identifier]);

        if (!texture->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        texture->setSmooth(true);
        return texture;
    }

    std::shared_ptr<sf::Font> ResourceContainer::LoadFont(const std::string& identifier, Uint8* data, Int64 size)
    {
        m_resources[identifier] = std::make_shared<sf::Font>();
        auto font = std::get<std::shared_ptr<sf::Font>>(m_resources[identifier]);

        if (!font->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        return font;
    }

    std::shared_ptr<sf::Texture> ResourceContainer::CacheTexture(const std::string& identifier, Uint8* data, Int64 size)
    {
        auto deleter = [=](sf::Texture* cache) { Uncache(identifier); delete cache; };
        auto texture = std::shared_ptr<sf::Texture>(new sf::Texture(), deleter);

        m_caches[identifier] = texture;
        if (!texture->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        texture->setSmooth(true);
        return texture;
    }

    std::shared_ptr<sf::Font> ResourceContainer::CacheFont(const std::string& identifier, Uint8* data, Int64 size)
    {
        auto deleter = [=](sf::Font* cache) { Uncache(identifier); delete cache; };
        auto font = std::shared_ptr<sf::Font>(new sf::Font(), deleter);
        
        m_caches[identifier] = font;
        if (!font->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        return font;
    }

    bool ResourceContainer::Uncache(const std::string& identifier)
    {
        auto iterator = m_caches.find(identifier);
        if (iterator != m_caches.end())
        {
            m_caches.erase(iterator);
            return true;
        }

        return false;
    }

    void ResourceContainer::Update(double delta)
    {
    }
}