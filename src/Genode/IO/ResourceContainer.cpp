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
        for (auto item : m_resources)
        {
            auto deleter = [](auto resource) {
                // Do not delete font, it's managed by internal font ref counters
                if (typeid(resource) != typeid(sf::Font*))
                    delete resource;
            };

            std::visit(deleter, item.second);
        }

        // Clearing cache will let weakptr died
        m_caches.clear();
        m_resources.clear();
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

    sf::Texture* ResourceContainer::LoadTexture(const std::string& identifier, Uint8* data, Int64 size)
    {
        m_resources[identifier] = new sf::Texture();
        auto texture = std::get<sf::Texture*>(m_resources[identifier]);

        if (!texture->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        texture->setSmooth(true);
        return texture;
    }

    sf::Font* ResourceContainer::LoadFont(const std::string& identifier, Uint8* data, Int64 size)
    {
        m_resources[identifier] = new sf::Font();
        auto font = std::get<sf::Font*>(m_resources[identifier]);

        if (!font->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        return font;
    }

    TextureCache ResourceContainer::CacheTexture(const std::string& identifier, Uint8* data, Int64 size)
    {
        auto deleter = [=](sf::Texture* cache) {
            Uncache(identifier);
            delete cache;
        };

        auto texture = std::shared_ptr<sf::Texture>(new sf::Texture(), deleter);
        m_caches[identifier] = texture;

        if (!texture->loadFromMemory(data, static_cast<size_t>(size)))
        {
            m_caches.erase(m_caches.find(identifier));
            return nullptr;
        }

        texture.get()->setSmooth(true);
        return texture;
    }

    FontCache ResourceContainer::CacheFont(const std::string& identifier, Uint8* data, Int64 size)
    {
        auto deleter = [=](sf::Font* cache) {
            Uncache(identifier);
            delete cache;
        };

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

    Int64 ResourceContainer::ReadResource(const std::string& path, Uint8** data)
    {
        if (m_fileSystem->Exists(path))
            return m_fileSystem->ReadFile(path, data);

        for (auto iterator : m_archives)
        {
            auto archive = iterator.second;
            if (archive && archive->Contains(path))
                return archive->GetFile(path, data);
        }
        
        return -1;
    }

    Int64 ResourceContainer::ReadResource(const std::string& filename, const std::string& identifier, Uint8** data)
    {
        auto name = m_fileSystem->GetFileName(filename);
        if (auto iterator = m_archives.find(name); iterator == m_archives.end())
            return -1;

        return m_archives[name]->GetFile(identifier, data);
    }

    void ResourceContainer::Update(double delta)
    {
    }
}