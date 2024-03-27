#ifndef GENODE_CACHE_HPP
#define GENODE_CACHE_HPP

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <variant>
#include <functional>
#include <vector>

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/DeserializerFactory.hpp>

#include <SFML/Audio/AlResource.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Gx
{
	typedef std::shared_ptr<sf::Texture>         TexturePtr;
	typedef std::shared_ptr<sf::Font>            FontPtr;
	typedef std::variant<
		std::weak_ptr<sf::Texture>,
		std::weak_ptr<sf::Font>>                 CacheEntry;
	typedef std::map<std::string, CacheEntry> CacheMap;

    class CacheManager
    {
    public:
		friend class ResourceManager;
        static CacheManager* Instance();

        template<class T>
        std::shared_ptr<T> Add(const std::string& name, Uint8* data, Int64 size);

        template<class T>
        std::shared_ptr<T> Get(const std::string& name) const;

        bool Remove(const std::string& name);

    private:
        CacheManager();
        ~CacheManager();

        CacheMap m_cacheMap;
    };
}

#include <Genode/IO/Cache.inl>
#endif