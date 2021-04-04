#ifndef GENODE_IO_CACHE_HPP
#define GENODE_IO_CACHE_HPP

#include <SFML/Audio/AlResource.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace Gx
{
    namespace priv
    {
        class BaseContainer
        {
        public:
            virtual ~BaseContainer() {}
        };
    }

    template<typename R>
    class ResourceContainer : public priv::BaseContainer
    {
    public:
        ResourceContainer();
        ~ResourceContainer();

        R* Add(const std::string& name, ResourcePtr<R> resource, bool useCache = true);
        R* Add(const std::string& name, std::function<ResourcePtr<R>()> resolver, bool useCache = true);
        bool Remove(R* resource);

        R* Find(const std::string& name) const;
        R& Get(const std::string& name) const;

        bool   Contains(const std::string& name) const;
        Uint64 Count() const;
        bool   Remove(const std::string& name);
        void   Clear();

    private:
        std::map<std::string, ResourcePtr<R>> m_caches;
    };
}

#include <Genode/IO/ResourceContainer.inl>
#endif