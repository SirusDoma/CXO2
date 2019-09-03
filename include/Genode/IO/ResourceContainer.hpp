#ifndef GENODE_RESOURCE_CONTAINER_HPP
#define GENODE_RESOURCE_CONTAINER_HPP

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <variant>
#include <functional>
#include <vector>

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/System/Module.hpp>

#include <SFML/Audio/AlResource.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Gx
{
    typedef std::variant<
        std::weak_ptr<sf::Texture>, 
        std::weak_ptr<sf::Font>> ResourceCache;

    typedef std::variant<
        sf::Texture*,
        sf::Font*> Resource;

    typedef std::shared_ptr<sf::Texture> TextureCache;
    typedef std::shared_ptr<sf::Font> FontCache;

    class ResourceContainer : public Module
    {
    public:
        static ResourceContainer* Instance();

        template<class T>
        T* AddArchive(const std::string& path);

        template<class T>
        T* AddArchive(T* archive);

        template<class T>
        T* Load(const std::string& path);

        template<class T>
        std::shared_ptr<T> Cache(const std::string& path);

        template<class T>
        T* Load(const std::string& filename, const std::string& identifier);

        template<class T>
        std::shared_ptr<T> Cache(const std::string& filename, const std::string& identifier);

        template<class T>
        T* Get(const std::string& identifier) const;  

        template<class T>
        std::shared_ptr<T> GetCache(const std::string& identifier) const;

        bool Remove(const std::string& identifier);
        bool Uncache(const std::string& identifier);

        const FileSystem* GetFileSystem() const;
        void SetFileSystem(const FileSystem* system);

        Int64 ReadResource(const std::string& path, Uint8** data);
        Int64 ReadResource(const std::string& filename, const std::string& identifier, Uint8** data);
    private:
        ResourceContainer();
        ~ResourceContainer();

        sf::Texture* LoadTexture(const std::string& identifier, Uint8* data, Int64 size);
        sf::Font* LoadFont(const std::string& identifier, Uint8* data, Int64 size);

        TextureCache CacheTexture(const std::string& identifier, Uint8* data, Int64 size);
        FontCache CacheFont(const std::string& identifier, Uint8* data, Int64 size);

        virtual void Update(double delta);

        const FileSystem* m_fileSystem;
        std::unordered_map<std::string, Archive*> m_archives;
        std::unordered_map<std::string, Resource> m_resources;
        std::map<std::string, ResourceCache>      m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
#endif