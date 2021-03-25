#ifndef GENODE_CACHE_HPP
#define GENODE_CACHE_HPP

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <SFML/Audio/AlResource.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

namespace Gx
{
    using CacheEntry = std::variant<
        std::unique_ptr<sf::Texture>,
        std::unique_ptr<sf::Font>,
        std::unique_ptr<sf::SoundBuffer>,
        std::unique_ptr<ResourceMetadata>,
        std::unique_ptr<Node>>;
    class CacheManager
    {
    public:
        CacheManager();
        ~CacheManager();

        template<typename T>
        T* Add(const std::string& name, Uint8* data, Int64 size, bool useCache = true);

        template<typename T>
        T* Add(const std::string& name, const T& value, bool useCache = true);

        template<typename T>
        T* Get(const std::string& name) const;

        bool   Contains(const std::string& name) const;
        Uint64 Count() const;
        bool   Remove(const std::string& name);
        void   Clear();

    private:
        std::map<std::string, std::shared_ptr<void>> m_cacheMap;
    };
}

#include <Genode/IO/CacheManager.inl>
#endif