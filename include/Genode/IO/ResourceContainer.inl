#include "ResourceContainer.hpp"
namespace Gx
{
    template<class T>
    inline T* ResourceContainer::AddArchive(const std::string& filename)
    {
        static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

        auto name = m_fileSystem->GetFileName(filename);
        if (auto archive = GetArchive<T>(name); archive != nullptr)
            return archive;

        m_archives[name] = new T();
        if (!m_archives[name]->Open(filename))
        {
            m_archives.erase(m_archives.find(name));
            return nullptr;
        }

        return reinterpret_cast<T*>(m_archives[name]);
    }

    template<class T>
    inline T* ResourceContainer::AddArchive(T* archive)
    {
        static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

        auto name = archive->GetName();
        if (auto resource = GetArchive<T>(name); resource != nullptr)
            return resource;

        return reinterpret_cast<T*>(m_archives[name]);
    }

    template<class T>
    inline T* ResourceContainer::GetArchive(const std::string& filename)
    {
        auto identifier = m_fileSystem->GetFileName(filename);
        if (auto iterator = m_archives.find(identifier); iterator != m_archives.end())
            return reinterpret_cast<T*>(m_archives[identifier]);

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Load(const std::string& path)
    {
        auto name = m_fileSystem->GetIdentifier(path);
        auto resource = Get<T>(name);
        if (resource)
            return resource;

        Uint8* data; Uint64 size;
        if (size = ReadResource(path, &data); size <= 0)
            return nullptr;

        if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(LoadTexture(name, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(LoadFont(name, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Load(const std::string& filename, const std::string& identifier)
    {
        auto resource = Get<T>(identifier);
        if (resource)
            return resource;

        Uint8* data; Uint64 size;
        if (size = ReadResource(filename, identifier, &data); size <= 0)
            return nullptr;

        if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(LoadTexture(identifier, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(LoadFont(identifier, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Cache(const std::string& path)
    {
        auto name = m_fileSystem->GetIdentifier(path);
        auto cache = GetCache<T>(name);
        if (cache)
            return cache;

        Uint8* data; Uint64 size;
        if (size = ReadResource(path, &data); size <= 0)
            return nullptr;

        if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(CacheTexture(name, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(CacheFont(name, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Cache(const std::string& filename, const std::string& identifier)
    {
        auto cache = GetCache<T>(identifier);
        if (cache)
            return cache;

        Uint8* data; Uint64 size;
        if (size = ReadResource(filename, identifier, &data); size <= 0)
            return nullptr;

        if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(CacheTexture(identifier, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(CacheFont(identifier, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Get(const std::string& name) const
    {
        auto identifier = m_fileSystem->GetIdentifier(name);
        auto iterator = m_resources.find(identifier);
        if (iterator != m_resources.end())
            return std::get<std::shared_ptr<T>>(iterator->second);

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::GetCache(const std::string& name) const
    {
        auto identifier = m_fileSystem->GetIdentifier(name);
        auto iterator = m_caches.find(identifier);
        if (iterator != m_caches.end())
        {
            auto cache = std::get<std::weak_ptr<T>>(iterator->second);
            if (!cache.expired())
                return cache.lock();
        }

        return nullptr;
    }
}