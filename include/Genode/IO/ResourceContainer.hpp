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
        std::shared_ptr<sf::Texture>,
        std::shared_ptr<sf::Font>> Resource;

    typedef std::shared_ptr<sf::Texture> TexturePtr;
    typedef std::shared_ptr<sf::Font> FontPtr;

    class ResourceContainer : public Module
    {
    public:
		friend class ResourceManager;
        static ResourceContainer* Instance();

        template<class T>
        std::shared_ptr<T> Load(const std::string& name, Uint8* data, Int64 size);

        template<class T>
        std::shared_ptr<T> Cache(const std::string& name, Uint8* data, Int64 size);

        template<class T>
        std::shared_ptr<T> Get(const std::string& name) const;

        template<class T>
        std::shared_ptr<T> GetCache(const std::string& name) const;

        bool Remove(const std::string& name);
        bool Uncache(const std::string& name);

        const FileSystem* GetFileSystem() const;
        void SetFileSystem(const FileSystem* system);

    private:
        ResourceContainer();
        ~ResourceContainer();

        TexturePtr LoadTexture(const std::string& identifier, Uint8* data, Int64 size);
        FontPtr LoadFont(const std::string& identifier, Uint8* data, Int64 size);

        TexturePtr CacheTexture(const std::string& identifier, Uint8* data, Int64 size);
        FontPtr CacheFont(const std::string& identifier, Uint8* data, Int64 size);

        virtual void Update(double delta);

        const FileSystem* m_fileSystem;
        std::unordered_map<std::string, Archive*> m_archives;
        std::unordered_map<std::string, Resource> m_resources;
        std::map<std::string, ResourceCache>      m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
#endif