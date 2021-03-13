#ifndef GENODE_DESERIALIZER_FACTORY_HPP
#define GENODE_DESERIALIZER_FACTORY_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <algorithm>

namespace Gx
{
    class ResourceLoaderFactory
    {
    public:
        template<typename T>
        static void Register(ResourceLoader<T>* deserializer);

        template<typename T>
        static void Register(MetadataLoader<T>* deserializer);

        template<typename T>
        static bool Remove();

        template<typename T>
        static ResourceLoader<T>* GetLoader();

        template<typename T>
        static MetadataLoader<T>* GetMetadataLoader();

    private:
        typedef std::unordered_map<std::type_index, priv::BaseLoader*> LoaderMap;
        static LoaderMap m_loaders;
    };
}

#include <Genode/IO/ResourceLoaderFactory.inl>
#endif
