#ifndef GENODE_CACHE_HPP
#define GENODE_CACHE_HPP

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/Audio/AlResource.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <variant>
#include <functional>
#include <vector>
#include <any>

namespace Gx
{
    typedef std::map<std::string, std::shared_ptr<void>> CacheMap;

    class CacheManager
    {
    public:
        CacheManager();
        ~CacheManager();

        template<typename T>
        std::shared_ptr<T> Add(const std::string& name, Uint8* data, Int64 size, bool useCache = true);

        template<typename T>
        std::shared_ptr<T> Add(const std::string& name, T* value, bool useCache = true);

        template<typename T>
        std::shared_ptr<T> Get(const std::string& name) const;

        bool   Contains(const std::string& name) const;
        Uint64 Count() const;
        bool   Remove(const std::string& name);
        void   Clear();

    private:
        CacheMap m_cacheMap;
    };
}

#include <Genode/IO/CacheManager.inl>
#endif